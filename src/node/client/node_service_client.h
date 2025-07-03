#ifndef SRC_NODE_CLIENT_NODE_SERVICE_CLIENT_H_
#define SRC_NODE_CLIENT_NODE_SERVICE_CLIENT_H_

#include <memory>
#include <grpcpp/grpcpp.h>

#include "src/node/node_service.grpc.pb.h"

class NodeServiceClient {
 public:
  explicit NodeServiceClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(proto::NodeService::NewStub(channel)) {}

  grpc::Status ScheduleJob(
      const proto::ScheduleJobRequest& request,
      proto::ScheduleJobResponse& response) const;

  grpc::Status GetResourceReport(
      const proto::GetResourceReportRequest& request,
      proto::GetResourceReportResponse& response) const;

  grpc::Status DoWork(
      const proto::DoWorkRequest& request,
      proto::DoWorkResponse& response) const;

  grpc::Status PollJobs(
      const proto::PollJobsRequest& request,
      proto::PollJobsResponse& response) const;

 private:
  std::unique_ptr<proto::NodeService::Stub> stub_;
};

#endif
