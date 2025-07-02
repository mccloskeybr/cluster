#ifndef SRC_NODE_SERVICE_NODE_SERVICE_IMPL_H_
#define SRC_NODE_SERVICE_NODE_SERVICE_IMPL_H_

#include <cstdint>
#include <string>
#include <grpcpp/grpcpp.h>

#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/usage_stats.h"

// TODO: on start, clear work directory. or automatically execute jobs in work directory.
class NodeServiceImpl final : public node::NodeService::Service {
  grpc::Status GetUsageReport(
      grpc::ServerContext* context,
      const node::GetUsageReportRequest* request,
      node::GetUsageReportResponse* response) override;

  grpc::Status DoWork(
      grpc::ServerContext* context,
      const node::DoWorkRequest* request,
      node::DoWorkResponse* response) override;

  grpc::Status PollJobs(
      grpc::ServerContext* context,
      const node::PollJobsRequest* request,
      node::PollJobsResponse* response) override;
};

#endif
