#include <grpcpp/grpcpp.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "absl/strings/str_format.h"
#include "status/protos/service.pb.h"

ABSL_FLAG(uint16_t port, 8080, "Server port for the service.");

class StatusServiceImpl final : public proto::Status::Service {
  grpc::Status HealthCheck(
      grpc::ServerContext* context,
      const proto::HealthCheckRequest* request,
      proto::HealthCheckResponse* response) override {
    return grpc::Status::OK;
  }
};

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  std::string server_address = absl::StrFormat("0.0.0.0:%d", absl::GetFlag(FLAGS_port));
  StatusServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  LOG(INFO) << "Service started on: " << server_address;

  server->Wait();
  return 0;
}
