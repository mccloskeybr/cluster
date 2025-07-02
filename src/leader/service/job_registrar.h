#ifndef SRC_LEADER_SERVICE_JOB_REGISTRAR_H_
#define SRC_LEADER_SERVICE_JOB_REGISTRAR_H_

#include <mutex>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"

class JobRegistrar {
 public:
  struct Entry {
    std::string name;
    std::string ip_addr;
    std::string port;
  };

  explicit JobRegistrar()
    : mutex_(), registry_() {}

  void RegisterJob(
      absl::string_view name, absl::string_view ip_addr, absl::string_view port);
  bool RemoveJob(
      absl::string_view name, absl::string_view ip_addr, absl::string_view port);
  std::vector<Entry> LookupJobsWithName(absl::string_view name);
  std::vector<Entry> LookupJobsWithIpAddr(absl::string_view ip_addr);

 private:
  std::mutex mutex_;
  std::vector<Entry> registry_;
};

#endif
