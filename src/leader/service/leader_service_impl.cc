#include "src/leader/service/leader_service_impl.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "src/leader/config.pb.h"
#include "src/node/client/node_service_client.h"

namespace leader {

grpc::Status LeaderServiceImpl::Create(
    std::unique_ptr<LeaderServiceImpl>& leader_service,
    std::string config_file_path) {
  leader::Config config;
  {
    std::ifstream config_file(config_file_path, std::ios::binary);
    if (!config_file.is_open()) {
      return grpc::Status(
          grpc::StatusCode::INVALID_ARGUMENT,
          absl::StrCat("Unable to open leader config file: ", config_file_path));
    }
    google::protobuf::io::IstreamInputStream input_stream(&config_file);
    google::protobuf::TextFormat::Parse(&input_stream, &config);
  }

  std::vector<node::NodeServiceClient> nodes;
  for (const leader::NodeAddress& node_address : config.nodes()) {
    const std::string target = absl::StrCat(
        node_address.ip_address(), ":", node_address.port());
    LOG(INFO) << "Registering node: " << target;
    auto client = node::NodeServiceClient(
        grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    nodes.push_back(std::move(client));
  }

  leader_service = std::make_unique<LeaderServiceImpl>(std::move(nodes));
  return grpc::Status::OK;
}

} // namespace leader
