#ifndef SRC_NODE_SERVICE_NODE_SERVICE_IMPL_H_
#define SRC_NODE_SERVICE_NODE_SERVICE_IMPL_H_

#include <cstdint>
#include <string>
#include <utility>
#include <grpcpp/grpcpp.h>

#include "src/node/client/node_service_client.h"
#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/util/job_registrar.h"
#include "src/node/service/util/node_registrar.h"

struct Node {
  std::string ip_addr;
  std::string port;
  NodeServiceClient client;
};

// TODO: on start, clear work directory. or automatically execute jobs in work directory.
class NodeServiceImpl final : public proto::NodeService::Service {
 public:
  NodeServiceImpl(const proto::NodeConfig& config)
      : node_registrar_(config), job_registrar_() {}
  ~NodeServiceImpl() = default;

  static grpc::Status CreateFromConfig(
      std::unique_ptr<NodeServiceImpl>& node_service,
      std::string config_file_path);

 private:
  grpc::Status ScheduleJob(
      grpc::ServerContext* context,
      const proto::ScheduleJobRequest* request,
      proto::ScheduleJobResponse* response) override;

  grpc::Status PollJobs(
      grpc::ServerContext* context,
      const proto::PollJobsRequest* request,
      proto::PollJobsResponse* response) override;

  grpc::Status GetResourceReport(
      grpc::ServerContext* context,
      const proto::GetResourceReportRequest* request,
      proto::GetResourceReportResponse* response) override;

  grpc::Status DoWork(
      grpc::ServerContext* context,
      const proto::DoWorkRequest* request,
      proto::DoWorkResponse* response) override;

  NodeRegistrar node_registrar_;
  JobRegistrar job_registrar_;
};

#endif
