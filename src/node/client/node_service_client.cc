#include "src/node/client/node_service_client.h"

#include <memory>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "src/node/node_service.grpc.pb.h"

grpc::Status NodeServiceClient::ScheduleJob(
    const proto::ScheduleJobRequest& request,
    proto::ScheduleJobResponse& response) const {
  grpc::ClientContext context;
  const grpc::Status status = stub_->ScheduleJob(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "Call to ScheduleJob failed: "
      << status.error_code() << " - " << status.error_message();
  }
  return status;
}

grpc::Status NodeServiceClient::GetResourceReport(
    const proto::GetResourceReportRequest& request,
    proto::GetResourceReportResponse& response) const {
  grpc::ClientContext context;
  const grpc::Status status = stub_->GetResourceReport(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "Call to GetResourceReport failed: "
      << status.error_code() << " - " << status.error_message();
  }
  return status;
}

grpc::Status NodeServiceClient::DoWork(
    const proto::DoWorkRequest& request,
    proto::DoWorkResponse& response) const {
  grpc::ClientContext context;
  const grpc::Status status = stub_->DoWork(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "Call to DoWork failed: " << request.job_name()
      << ": " << status.error_code() << " - " << status.error_message();
  }
  return status;
}

grpc::Status NodeServiceClient::PollJobs(
    const proto::PollJobsRequest& request,
    proto::PollJobsResponse& response) const {
  grpc::ClientContext context;
  const grpc::Status status = stub_->PollJobs(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "Call to PollJobs failed: "
      << status.error_code() << " - " << status.error_message();
  }
  return status;
}
