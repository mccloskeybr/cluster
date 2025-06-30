#include "src/node/service/usage_stats.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "absl/strings/str_cat.h"

namespace node {

UsageStats UsageStats::Collect() {
    std::string cpu_label;
    uint64_t user, nice, system, idle, iowait;
    uint64_t irq, softirq, steal, guest, guest_nice;
    {
      std::ifstream file("/proc/stat");
      std::string line;
      std::getline(file, line);
      std::istringstream iss(line);
      iss >> cpu_label >> user >> nice >> system >> idle >> iowait;
      iss >> irq >> softirq >> steal >> guest >> guest_nice;
    }
    UsageStats stats;
    stats.idle = idle + iowait;
    stats.active = user + nice + system + irq + softirq + steal + guest + guest_nice;
    stats.total = stats.idle + stats.active;
    return stats;
}

std::string UsageStats::DebugString() const {
  return absl::StrCat(
      "UsageStats { idle: ", idle,
      ", active: ", active,
      ", total: ", total, " }");
}

} // namespace node
