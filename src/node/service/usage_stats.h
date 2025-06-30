#ifndef SRC_NODE_SERVICE_USAGE_STATS_H_
#define SRC_NODE_SERVICE_USAGE_STATS_H_

#include <cstdint>
#include <string>

namespace node {

struct UsageStats {
  static UsageStats Collect();
  std::string DebugString() const;

  uint64_t idle;
  uint64_t active;
  uint64_t total;
};

} // namespace node

#endif
