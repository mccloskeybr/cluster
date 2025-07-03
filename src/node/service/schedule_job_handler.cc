#include "src/node/service/node_service_impl.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/util/node_registrar.h"

grpc::Status NodeServiceImpl::ScheduleJob(
    grpc::ServerContext* context,
    const proto::ScheduleJobRequest* request,
    proto::ScheduleJobResponse* response) {
  // NOTE: find and read executable.
  const static std::string kExecDir = absl::StrCat(std::getenv("HOME"), "/binaries/");
  const std::string exec_file = absl::StrCat(kExecDir, request->job_name());
  if (!std::filesystem::exists(exec_file)) {
    return grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        absl::StrCat("Job does not exist: ", exec_file));
  }
  std::string exec_raw;
  {
    std::ifstream file(exec_file, std::ios::binary);
    if (!file.is_open()) {
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          absl::StrCat("Unable to open executable: ", exec_file));
    }
    file.seekg(0, std::ios_base::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios_base::beg);
    exec_raw = std::string(file_size, '\0');
    file.read(&exec_raw[0], file_size);
  }

  // NOTE: find node to schedule.
  const NodeMetadata* node = nullptr;

  // NOTE: try to find a local node with enough space.
  std::vector<NodeMetadata> nodes = node_registrar_.GetSnapshot();
  float max_idle = std::numeric_limits<float>::min();
  for (size_t i = 0; i < nodes.size(); i++) {
    if (nodes[i].state != NodeState::HEALTHY) { continue; }
    if (nodes[i].local_stats.idle < request->min_cpu()) { continue; }
    if (nodes[i].local_stats.idle > max_idle) {
      max_idle = nodes[i].local_stats.idle;
      node = &nodes[i];
    }
  }

  // NOTE: try to fall back on a node with enough aggregate space to forward the request to.
  if (node == nullptr) {
    LOG(INFO) << "Not enough space for " << request->min_cpu()
      << " among local nodes, attempting to forward...";
    for (size_t i = 0; i < nodes.size(); i++) {
      if (nodes[i].is_self) { continue; }
      if (nodes[i].state != NodeState::HEALTHY) { continue; }
      if (nodes[i].aggregate_stats.idle < request->min_cpu()) { continue; }
      if (nodes[i].aggregate_stats.idle > max_idle) {
        max_idle = nodes[i].local_stats.idle;
        node = &nodes[i];
      }
    }
    LOG(INFO) << "No child node found that has enough space for " << request->min_cpu();
    if (node == nullptr) {
      return grpc::Status(
          grpc::StatusCode::UNAVAILABLE,
          absl::StrCat("No node found with available resources: ", request->min_cpu(), "!"));
    }
    LOG(INFO) << "Forwarding scheduling request to child node: " << node->ip_addr;
    return node->client->ScheduleJob(*request, *response);
  }
  LOG(INFO) << "Scheduling job: " << request->job_name()
    << " on node: " << node->ip_addr << " with CPU idle: " << max_idle;

  // NOTE: submit scheduling request.
  // TODO: retry failed requests by trying the next node. sort nodes by cpu usage.
  proto::DoWorkRequest do_work_request;
  *do_work_request.mutable_job_name() = request->job_name();
  *do_work_request.mutable_port() = request->port();
  *do_work_request.mutable_args() = request->args();
  *do_work_request.mutable_executable() = exec_raw;
  proto::DoWorkResponse do_work_response;
  grpc::Status status = node->client->DoWork(do_work_request, do_work_response);
  if (!status.ok()) { return status; }

  *response->mutable_ip_addr() = node->ip_addr;
  return grpc::Status::OK;
}
