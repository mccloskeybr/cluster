#include "src/node/service/util/node_registrar.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>
#include <thread>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include <grpcpp/grpcpp.h>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "src/node/config.pb.h"
#include "src/node/client/node_service_client.h"
#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/util/resource_stats.h"
#include "src/node/service/util/network.h"

namespace {

absl::string_view NodeStateToString(NodeState state) {
  using enum NodeState;
  switch (state) {
    case UNDEFINED: return "UNDEFINED";
    case UNHEALTHY: return "UNHEALTHY";
    case HEALTHY: return "HEALTHY";
    default: CHECK(false) << "Unsupported NodeState detected."; return "";
  }
}

std::pair<grpc::Status, proto::GetResourceReportResponse> GetResourceReport(const NodeMetadata& node) {
  proto::GetResourceReportRequest request;
  proto::GetResourceReportResponse response;
  grpc::Status status = node.client->GetResourceReport(request, response);
  if (!status.ok()) {
    LOG(ERROR) << "Unable to get node resource report: " << node.ip_addr
    << ": " << status.error_code() << ": " << status.error_message();
  }
  return std::make_pair(status, response);
}

} // namespace

void PollNodeStatus(NodeRegistrar* registrar) {
  const auto kPollFrequency = std::chrono::seconds(30);
  while (!registrar->stop_threads_) {
    {
      LOG(INFO) << "Polling health of connected nodes...";
      std::scoped_lock lock(registrar->mutex_);

      // NOTE: Polling resource statistics can be long (seconds), so thread them to minimize lock duration.
      std::vector<std::future<std::pair<grpc::Status, proto::GetResourceReportResponse>>> reports(registrar->nodes_.size());
      for (size_t i = 0; i < registrar->nodes_.size(); i++) {
        IsSelfIpAddr(registrar->nodes_[i].ip_addr);
        reports[i] = std::async(std::launch::async, GetResourceReport, registrar->nodes_[i]);
      }

      ResourceStats aggregate_stats = {};
      for (size_t i = 0; i < reports.size(); i++) {
        auto report = reports[i].get();
        NodeMetadata& node = registrar->nodes_[i];
        node.state = std::get<0>(report).ok() ? NodeState::HEALTHY : NodeState::UNHEALTHY;
        node.local_stats = ResourceStats::FromProto(std::get<1>(report).local_stats());
        node.aggregate_stats = ResourceStats::FromProto(std::get<1>(report).aggregate_stats());

        aggregate_stats = aggregate_stats + node.local_stats;
        if (!node.is_self) { aggregate_stats = aggregate_stats + node.aggregate_stats; }

        LOG(INFO) << "Node health report: " << node.ip_addr << ":" << node.port << " - "
          << NodeStateToString(node.state) << ", stats: " << node.local_stats.DebugString();
      }
      LOG(INFO) << "Collected aggregate stats for node: " << aggregate_stats.DebugString();
      registrar->aggregate_stats_.store(aggregate_stats);
    }
    std::this_thread::sleep_for(kPollFrequency);
  }
}

NodeRegistrar::NodeRegistrar(const proto::NodeConfig& config)
    : mutex_(), nodes_(), threads_(), stop_threads_(false), aggregate_stats_() {
  nodes_.reserve(config.nodes().size());
  for (const proto::NodeConfig::NodeAddress& node_address : config.nodes()) {
    LOG(INFO) << "Registering node: "
      << node_address.ip_address() << ":" << node_address.port();
    NodeMetadata node = {
      .ip_addr = node_address.ip_address(),
      .port = node_address.port(),
      .client = std::make_shared<NodeServiceClient>(grpc::CreateChannel(
            absl::StrCat(node_address.ip_address(), ":", node_address.port()),
            grpc::InsecureChannelCredentials())),
      .is_self = IsSelfIpAddr(node_address.ip_address()),
    };
    nodes_.push_back(std::move(node));
  }

  threads_.push_back(std::thread(PollNodeStatus, this));
}

NodeRegistrar::~NodeRegistrar() {
  stop_threads_ = true;
  for (std::thread& thread : threads_) { thread.join(); }
}

std::vector<NodeMetadata> NodeRegistrar::GetSnapshot() {
  std::scoped_lock lock(mutex_);
  return nodes_;
}

ResourceStats NodeRegistrar::GetAggregateStats() {
  return aggregate_stats_.load();
}
