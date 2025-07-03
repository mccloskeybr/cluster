#include "src/node/service/util/network.h"

#include <arpa/inet.h>
#include <cstdint>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string>
#include <sys/types.h>
#include <grpcpp/grpcpp.h>

#include "absl/log/check.h"
#include "absl/strings/string_view.h"

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

bool IsSelfIpAddr(absl::string_view ip_addr) {
  struct ifaddrs *if_addrs = nullptr;
  CHECK(getifaddrs(&if_addrs) != -1) << "Unable to get IP addresses.";

  bool found_match = false;
  for (struct ifaddrs* ifa = if_addrs; ifa != nullptr; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) { continue; }

    void* tmp_addr = nullptr;
    char address_buff[INET6_ADDRSTRLEN];
    if (ifa->ifa_addr->sa_family == AF_INET) {
        tmp_addr = &((struct sockaddr_in*) ifa->ifa_addr)->sin_addr;
        memset(address_buff, 0, INET6_ADDRSTRLEN); // Clear buffer
        inet_ntop(AF_INET, tmp_addr, address_buff, INET_ADDRSTRLEN);
        if (address_buff == ip_addr) { found_match = true; break; }
    } else if (ifa->ifa_addr->sa_family == AF_INET6) {
        tmp_addr = &((struct sockaddr_in6*) ifa->ifa_addr)->sin6_addr;
        memset(address_buff, 0, INET6_ADDRSTRLEN); // Clear buffer
        inet_ntop(AF_INET6, tmp_addr, address_buff, INET6_ADDRSTRLEN);
        if (address_buff == ip_addr) { found_match = true; break; }
    }
  }
  if (if_addrs != nullptr) { freeifaddrs(if_addrs); }
  return found_match;
}
