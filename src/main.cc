#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <memory>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/strings/str_format.h"
#include "src/leader/service/leader_service_impl.h"
#include "src/node/service/node_service_impl.h"

ABSL_FLAG(uint16_t, port, 6000,
    "Port to accept gRPC traffic from.");
ABSL_FLAG(std::string, leader_config_path, "",
    "Path to the leader configuration. If populated, assumed that this node is a leader.");

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  grpc::reflection::InitProtoReflectionServerBuilderPlugin();

  NodeServiceImpl node_service;
  std::unique_ptr<LeaderServiceImpl> leader_service = nullptr;

  grpc::ServerBuilder builder;
  builder.RegisterService(&node_service);
  if (!absl::GetFlag(FLAGS_leader_config_path).empty()) {
    LOG(INFO) << "Registering LeaderService.";
    const grpc::Status status = LeaderServiceImpl::Create(
        leader_service, absl::GetFlag(FLAGS_leader_config_path));
    if (!status.ok()) {
      LOG(ERROR) << status.error_code() << ": " << status.error_message();
      exit(1);
    }
    builder.RegisterService(leader_service.get());
  }

  std::string server_address = absl::StrFormat("0.0.0.0:%d", absl::GetFlag(FLAGS_port));
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOG(INFO) << "Server started on: " << server_address;

  server->Wait();
  return 0;
}
