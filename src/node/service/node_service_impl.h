#ifndef SRC_NODE_SERVICE_NODE_SERVICE_IMPL_H_
#define SRC_NODE_SERVICE_NODE_SERVICE_IMPL_H_

#include <cstdint>
#include <string>
#include <grpcpp/grpcpp.h>

#include "src/node/client/node_service_client.h"
#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/util/job_registrar.h"

struct Node {
  std::string ip_addr;
  std::string port;
  NodeServiceClient client;
};

// TODO: on start, clear work directory. or automatically execute jobs in work directory.
class NodeServiceImpl final : public node::NodeService::Service {
 public:
  NodeServiceImpl(std::vector<Node> nodes)
      : nodes_(std::move(nodes)), job_registrar_() {}
  ~NodeServiceImpl() = default;

  static grpc::Status Create(
      std::unique_ptr<NodeServiceImpl>& node_service,
      std::string config_file_path);

 private:
  grpc::Status ResolveName(
      grpc::ServerContext* context,
      const node::ResolveNameRequest* request,
      node::ResolveNameResponse* response) override;

  grpc::Status ScheduleJob(
      grpc::ServerContext* context,
      const node::ScheduleJobRequest* request,
      node::ScheduleJobResponse* response) override;

  grpc::Status PollJobs(
      grpc::ServerContext* context,
      const node::PollJobsRequest* request,
      node::PollJobsResponse* response) override;

  grpc::Status GetUsageReport(
      grpc::ServerContext* context,
      const node::GetUsageReportRequest* request,
      node::GetUsageReportResponse* response) override;

  grpc::Status DoWork(
      grpc::ServerContext* context,
      const node::DoWorkRequest* request,
      node::DoWorkResponse* response) override;

  const std::vector<Node> nodes_;
  JobRegistrar job_registrar_;
};

#endif
