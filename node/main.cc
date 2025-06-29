#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "node/node_service.grpc.pb.h"

ABSL_FLAG(uint16_t, port, 8080, "Server port for the service.");

class NodeServiceImpl final : public node::NodeService::Service {
 public:
  explicit NodeServiceImpl() {
    hi = "hello";
    LOG(INFO) << "what's up?";
  }

 private:
  grpc::Status HealthCheck(
      grpc::ServerContext* context,
      const node::HealthCheckRequest* request,
      node::HealthCheckResponse* response) override {
    LOG(INFO) << hi;
    return grpc::Status::OK;
  }

  std::string hi;
};

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  NodeServiceImpl node_service;

  grpc::ServerBuilder builder;
  std::string server_address = absl::StrFormat("0.0.0.0:%d", absl::GetFlag(FLAGS_port));
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&node_service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOG(INFO) << "Service started on: " << server_address;

  server->Wait();
  return 0;
}
