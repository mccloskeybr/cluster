#include "src/node/service/node_service_impl.h"

#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "src/node/node_service.grpc.pb.h"

grpc::Status NodeServiceImpl::ResolveName(
    grpc::ServerContext* context,
    const node::ResolveNameRequest* request,
    node::ResolveNameResponse* response) {
  // TODO: query job registrar
  // if not present, look at adjacent nodes
  // check to make sure that node ip is not this node's ip, and is not the caller's ip
  // need to probably save all ip addresses separately. e.g. what if configured is wlan but get eth.
  return grpc::Status::OK;
}
