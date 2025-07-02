#include "src/leader/service/leader_service_impl.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "src/leader/config.pb.h"
#include "src/leader/service/job_registrar.h"
#include "src/node/client/node_service_client.h"

// TODO: nodes should also push updates to the leader
void PollNodes(LeaderServiceImpl* leader) {
  static const auto kSleepDuration = std::chrono::seconds(30);
  while (!leader->terminate_threads_) {
    for (const Node& node : leader->nodes_) {
      node::PollJobsRequest request;
      node::PollJobsResponse response;
      const grpc::Status status = node.client.PollJobs(request, response);
      if (!status.ok()) {
        LOG(ERROR) << "Unable to poll jobs on node: "
          << node.ip_addr << ":" << node.port;
        continue;
      }

      // TODO: also consider port!
      // TODO: also consider reverse, see if there are jobs that should be registered that arent?
      // likely want to store a registry locally on the node for this, for port info?
      // then instead of reading the directory we just read the local registry.

      const std::vector<JobRegistrar::Entry> registered_jobs =
        leader->job_registrar_.LookupJobsWithIpAddr(node.ip_addr);
      for (const JobRegistrar::Entry& registered_job : registered_jobs) {
        bool found = false;
        for (absl::string_view job_name : response.jobs()) {
          if (job_name == registered_job.name) {
            found = true;
            break;
          }
        }
        if (found == false) {
          LOG(INFO) << "Removing stale job: " << registered_job.name
            << " from node: " << node.ip_addr << ":" << node.port;
          leader->job_registrar_.RemoveJob(
              registered_job.name, registered_job.ip_addr, registered_job.port);
        }
      }
    }

    std::this_thread::sleep_for(kSleepDuration);
  }
}

grpc::Status LeaderServiceImpl::Create(
    std::unique_ptr<LeaderServiceImpl>& leader_service,
    std::string config_file_path) {
  leader::Config config;
  {
    std::ifstream config_file(config_file_path, std::ios::binary);
    if (!config_file.is_open()) {
      return grpc::Status(
          grpc::StatusCode::INVALID_ARGUMENT,
          absl::StrCat("Unable to open leader config file: ", config_file_path));
    }
    google::protobuf::io::IstreamInputStream input_stream(&config_file);
    google::protobuf::TextFormat::Parse(&input_stream, &config);
  }

  std::vector<Node> nodes;
  nodes.reserve(config.nodes().size());
  for (const leader::Config::NodeAddress& node_address : config.nodes()) {
    LOG(INFO) << "Registering node: "
      << node_address.ip_address() << ":" << node_address.port();
    Node node = {
      .ip_addr = node_address.ip_address(),
      .port = node_address.port(),
      .client = NodeServiceClient(grpc::CreateChannel(
            absl::StrCat(node_address.ip_address(), ":", node_address.port()),
            grpc::InsecureChannelCredentials())),
    };
    nodes.push_back(std::move(node));
  }

  leader_service = std::make_unique<LeaderServiceImpl>(std::move(nodes));
  return grpc::Status::OK;
}

void LeaderServiceImpl::RegisterThread(std::thread thread) {
}

LeaderServiceImpl::LeaderServiceImpl(std::vector<Node> nodes)
    : nodes_(std::move(nodes)), threads_(), job_registrar_() {
  threads_.push_back(std::thread(PollNodes, this));
}

LeaderServiceImpl::~LeaderServiceImpl() {
  terminate_threads_ = true;
  for (std::thread& thread : threads_) {
    thread.join();
  }
}
