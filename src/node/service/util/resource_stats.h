#ifndef SRC_NODE_SERVICE_UTIL_USAGE_STATS_H_
#define SRC_NODE_SERVICE_UTIL_USAGE_STATS_H_

#include <cstdint>
#include <string>

#include "src/node/node_service.grpc.pb.h"

// NOTE: this uses the Linux API to get OS / hardware snapshots.
// Therefore, it is only useful to use this when comparing 2 snapshots
// taken over some time period, rather than a single one in isolation.
struct ResourceStats {
  static ResourceStats Collect();
  static proto::ResourceStats ToProto(const ResourceStats& stats);
  static ResourceStats FromProto(const proto::ResourceStats& stats);
  std::string DebugString() const;

  ResourceStats operator+(const ResourceStats& other) const;
  ResourceStats operator-(const ResourceStats& other) const;
  ResourceStats operator/(int64_t scalar) const;

  // NOTE: units are contextual!
  float idle;
  float active;
  float total;
};

#endif
