#ifndef SRC_NODE_SERVICE_UTIL_NETWORK_H_
#define SRC_NODE_SERVICE_UTIL_NETWORK_H_

#include <string>
#include <grpcpp/grpcpp.h>

#include "absl/strings/string_view.h"

std::string GetPeerIpAddr(grpc::ServerContext* context);
bool IsSelfIpAddr(absl::string_view ip_addr);

#endif
