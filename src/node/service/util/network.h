#ifndef SRC_NODE_SERVICE_UTIL_NETWORK_H_
#define SRC_NODE_SERVICE_UTIL_NETWORK_H_

#include <string>
#include <grpcpp/grpcpp.h>

// Extracts the peer IP address.
std::string GetPeerIpAddr(grpc::ServerContext* context);

#endif
