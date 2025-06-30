#ifndef SRC_LEADER_SERVICE_LEADER_SERVICE_IMPL_H_
#define SRC_LEADER_SERVICE_LEADER_SERVICE_IMPL_H_

#include <memory>
#include <grpcpp/grpcpp.h>
#include <string>
#include <vector>

#include "src/leader/leader_service.grpc.pb.h"
#include "src/node/client/node_service_client.h"

namespace leader {

class LeaderServiceImpl final : public leader::LeaderService::Service {
 public:
  LeaderServiceImpl(std::vector<node::NodeServiceClient> nodes)
    : nodes_(std::move(nodes)) {}

  static grpc::Status Create(
      std::unique_ptr<LeaderServiceImpl>& leader_service,
      std::string config_file_path);

 private:
  grpc::Status ScheduleJob(
      grpc::ServerContext* context,
      const leader::ScheduleJobRequest* request,
      leader::ScheduleJobResponse* response) override;

  const std::vector<node::NodeServiceClient> nodes_;
};

} // namespace leader

#endif
