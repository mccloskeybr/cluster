#include "src/node/service/node_service_impl.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "src/node/client/node_service_client.h"
#include "src/node/config.pb.h"
#include "src/node/service/util/job_registrar.h"

grpc::Status NodeServiceImpl::Create(
    std::unique_ptr<NodeServiceImpl>& node_service,
    std::string config_file_path) {
  node::NodeConfig config;
  {
    std::ifstream config_file(config_file_path, std::ios::binary);
    if (!config_file.is_open()) {
      return grpc::Status(
          grpc::StatusCode::INVALID_ARGUMENT,
          absl::StrCat("Unable to open node config file: ", config_file_path));
    }
    google::protobuf::io::IstreamInputStream input_stream(&config_file);
    google::protobuf::TextFormat::Parse(&input_stream, &config);
  }

  std::vector<Node> nodes;
  nodes.reserve(config.nodes().size());
  for (const node::NodeConfig::NodeAddress& node_address : config.nodes()) {
    LOG(INFO) << "Registering node: "
      << node_address.ip_address() << ":" << node_address.port();
    Node node = {
      .ip_addr = node_address.ip_address(),
      .port = node_address.port(),
      .client = NodeServiceClient(grpc::CreateChannel(
            absl::StrCat(node_address.ip_address(), ":", node_address.port()),
            grpc::InsecureChannelCredentials())),
    };
    nodes.push_back(std::move(node));
  }

  node_service = std::make_unique<NodeServiceImpl>(std::move(nodes));
  return grpc::Status::OK;
}
