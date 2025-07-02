#ifndef SRC_NODE_CLIENT_NODE_SERVICE_CLIENT_H_
#define SRC_NODE_CLIENT_NODE_SERVICE_CLIENT_H_

#include <memory>
#include <grpcpp/grpcpp.h>

#include "src/node/node_service.grpc.pb.h"

class NodeServiceClient {
 public:
  explicit NodeServiceClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(node::NodeService::NewStub(channel)) {}

  grpc::Status GetUsageReport(
      const node::GetUsageReportRequest& request,
      node::GetUsageReportResponse& response) const;

  grpc::Status DoWork(
      const node::DoWorkRequest& request,
      node::DoWorkResponse& response) const;

  grpc::Status PollJobs(
      const node::PollJobsRequest & request,
      node::PollJobsResponse& response) const;

 private:
  std::unique_ptr<node::NodeService::Stub> stub_;
};

#endif
