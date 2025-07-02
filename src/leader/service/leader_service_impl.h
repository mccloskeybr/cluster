#ifndef SRC_LEADER_SERVICE_LEADER_SERVICE_IMPL_H_
#define SRC_LEADER_SERVICE_LEADER_SERVICE_IMPL_H_

#include <memory>
#include <grpcpp/grpcpp.h>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "src/leader/leader_service.grpc.pb.h"
#include "src/leader/service/job_registrar.h"
#include "src/node/client/node_service_client.h"

struct Node {
  std::string ip_addr;
  std::string port;
  NodeServiceClient client;
};

class LeaderServiceImpl final : public leader::LeaderService::Service {
 public:
  LeaderServiceImpl(std::vector<Node> nodes);
  ~LeaderServiceImpl();

  static grpc::Status Create(
      std::unique_ptr<LeaderServiceImpl>& leader_service,
      std::string config_file_path);

  void RegisterThread(std::thread thread);

 private:
  grpc::Status ResolveName(
      grpc::ServerContext* context,
      const leader::ResolveNameRequest* request,
      leader::ResolveNameResponse* response) override;

  grpc::Status ScheduleJob(
      grpc::ServerContext* context,
      const leader::ScheduleJobRequest* request,
      leader::ScheduleJobResponse* response) override;

  const std::vector<Node> nodes_;
  std::vector<std::thread> threads_;
  std::atomic<bool> terminate_threads_;
  JobRegistrar job_registrar_;

  friend void PollNodes(LeaderServiceImpl*);
};

#endif
