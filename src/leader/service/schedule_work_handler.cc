#include "src/leader/service/leader_service_impl.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "src/leader/leader_service.grpc.pb.h"

namespace leader {

grpc::Status LeaderServiceImpl::ScheduleJob(
    grpc::ServerContext* context,
    const leader::ScheduleJobRequest* request,
    leader::ScheduleJobResponse* response) {
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
  size_t min_cpu_idx = std::numeric_limits<size_t>::max();
  float min_cpu_usage = std::numeric_limits<float>::max();;
  for (size_t i = 0; i < nodes_.size(); i++) {
    node::GetUsageReportRequest get_usage_request;
    node::GetUsageReportResponse get_usage_response;
    grpc::Status status = nodes_[i].GetUsageReport(get_usage_request, get_usage_response);
    if (!status.ok()) { continue; }
    if (get_usage_response.cpu_usage() < min_cpu_usage) {
      min_cpu_usage = get_usage_response.cpu_usage();
      min_cpu_idx = i;
    }
  }
  if (min_cpu_idx == std::numeric_limits<size_t>::max()) {
    return grpc::Status(
        grpc::StatusCode::UNAVAILABLE,
        "Unable to find an available node.");
  }

  LOG(INFO) << "Chose idx: " << min_cpu_idx << " with CPU usage: " << min_cpu_usage;

  // NOTE: submit scheduling request.
  node::DoWorkRequest do_work_request;
  *do_work_request.mutable_job_name() = request->job_name();
  *do_work_request.mutable_args() = request->args();
  do_work_request.set_executable(exec_raw);
  node::DoWorkResponse do_work_response;
  grpc::Status status = nodes_[min_cpu_idx].DoWork(do_work_request, do_work_response);
  if (!status.ok()) { return status; }

  return grpc::Status::OK;
}

} // namespace node
