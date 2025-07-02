#include "src/leader/service/leader_service_impl.h"

#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "src/leader/leader_service.grpc.pb.h"

grpc::Status LeaderServiceImpl::ResolveName(
    grpc::ServerContext* context,
    const leader::ResolveNameRequest* request,
    leader::ResolveNameResponse* response) {
  const std::vector<JobRegistrar::Entry> jobs =
    job_registrar_.LookupJobsWithName(request->name());
  if (jobs.empty()) {
    return grpc::Status(
        grpc::StatusCode::NOT_FOUND,
        absl::StrCat("Job with name: ", request->name()," was not found / able to be resolved."));
  }

  // TODO: verify node is alive?
  // TODO: load balance?
  const JobRegistrar::Entry& job = jobs[0];
  LOG(INFO) << "Job with name: " << request->name() << " resolved to: "
    << job.ip_addr << ":" << job.port;
  *response->mutable_ip_addr() = job.ip_addr;
  *response->mutable_port() = job.port;

  return grpc::Status::OK;
}
