#include "src/node/client/node_service_client.h"

#include <memory>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "src/node/node_service.grpc.pb.h"

namespace node {

grpc::Status NodeServiceClient::GetUsageReport(
    const node::GetUsageReportRequest& request,
    node::GetUsageReportResponse& response) const {
  grpc::ClientContext context;
  const grpc::Status status = stub_->GetUsageReport(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "Call to GetUsageReport failed: "
      << status.error_code() << " - " << status.error_message();
  }
  return status;
}

grpc::Status NodeServiceClient::DoWork(
    const node::DoWorkRequest& request,
    node::DoWorkResponse& response) const {
  grpc::ClientContext context;
  const grpc::Status status = stub_->DoWork(&context, request, &response);
  if (!status.ok()) {
    LOG(ERROR) << "Call to DoWork failed: " << request.job_name()
      << ": " << status.error_code() << " - " << status.error_message();
  }
  return status;
}

} // namespace node
