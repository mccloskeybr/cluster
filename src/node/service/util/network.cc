#include "src/node/service/util/network.h"

#include <cstdint>
#include <string>
#include <grpcpp/grpcpp.h>

#include "absl/log/check.h"

std::string GetPeerIpAddr(grpc::ServerContext* context) {
  size_t start = 0, end = 0;
  std::string peer = context->peer();
  for (size_t i = 0; i < peer.size(); i++) {
    if (peer[i] == ':') {
      if (start == 0) { start = i + 1; }
      else { end = i; break; }
    }
  }
  CHECK(start != 0 && end != 0) << "Unexpectedly unable to extract ip from: " << peer;
  return peer.substr(start, end - start);
}
