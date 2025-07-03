#include "src/node/service/util/resource_stats.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "src/node/node_service.grpc.pb.h"

namespace {

float GetAvgCpuFreqGHz() {
  size_t agg_freq_khz = 0.0;
  size_t num_cores = 0;
  for (int i = 0; true; i++) {
    std::string filename = "/sys/devices/system/cpu/cpu" + std::to_string(i) + "/cpufreq/cpuinfo_max_freq";
    std::ifstream freq_file(filename);
    if (!freq_file.is_open()) { break; }
    std::string freq_khz;
    std::getline(freq_file, freq_khz);
    agg_freq_khz += std::stoi(freq_khz);
    num_cores++;
  }
  CHECK(agg_freq_khz > 0) << "No CPUs found!";
  float avg_freq = (agg_freq_khz / 1000000.0) / num_cores;
  LOG(INFO) << "Determined average CPU speed: " << avg_freq;
  return avg_freq;
}

float ToGhz(int64_t jiffies) {
  static const size_t kJiffyHz = sysconf(_SC_CLK_TCK);
  static const float kAvgCpuFreqGHz = GetAvgCpuFreqGHz();
  return ((((float) jiffies) / kJiffyHz) * kAvgCpuFreqGHz);
}

} // namespace

ResourceStats ResourceStats::Collect() {
  std::string cpu_label;
  size_t user, nice, system, idle, iowait;
  size_t irq, softirq, steal, guest, guest_nice;

  std::ifstream file("/proc/stat");
  std::string line;
  std::getline(file, line);
  std::istringstream iss(line);
  iss >> cpu_label >> user >> nice >> system >> idle >> iowait;
  iss >> irq >> softirq >> steal >> guest >> guest_nice;

  int64_t idle_jiffies = (idle + iowait);
  int64_t active_jiffies = (user + nice + system + irq + softirq + steal + guest + guest_nice);

  return ResourceStats {
    .idle = ToGhz(idle_jiffies),
    .active = ToGhz(active_jiffies),
    .total = ToGhz(idle_jiffies + active_jiffies),
  };
}

ResourceStats ResourceStats::operator+(const ResourceStats& other) const {
  return ResourceStats {
    .idle = idle + other.idle,
    .active = active + other.active,
    .total = total + other.total,
  };
}

ResourceStats ResourceStats::operator-(const ResourceStats& other) const {
  return ResourceStats {
    .idle = idle - other.idle,
    .active = active - other.active,
    .total = total - other.total,
  };
}

ResourceStats ResourceStats::operator/(int64_t scalar) const {
  float x = (float) scalar;
  return ResourceStats {
    .idle = idle / x,
    .active = active / x,
    .total = total / x,
  };
}

proto::ResourceStats ResourceStats::ToProto(const ResourceStats& stats) {
  proto::ResourceStats result;
  result.set_idle_cpu_seconds(stats.idle);
  result.set_active_cpu_seconds(stats.active);
  result.set_total_cpu_seconds(stats.total);
  return result;
}

ResourceStats ResourceStats::FromProto(const proto::ResourceStats& stats) {
  return ResourceStats {
    .idle = stats.idle_cpu_seconds(),
    .active = stats.active_cpu_seconds(),
    .total = stats.total_cpu_seconds(),
  };
}

std::string ResourceStats::DebugString() const {
  return absl::StrCat(
      "ResourceStats { ",
      "idle: ",   idle,   ", ",
      "active: ", active, ", ",
      "total: ",  total,  " }");
}
