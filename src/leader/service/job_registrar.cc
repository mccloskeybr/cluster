#include "src/leader/service/job_registrar.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"

void JobRegistrar::RegisterJob(
    absl::string_view name, absl::string_view ip_addr, absl::string_view port) {
  JobRegistrar::Entry entry = {
    .name = std::string(name),
    .ip_addr = std::string(ip_addr),
    .port = std::string(port),
  };
  std::scoped_lock lock(mutex_);
  registry_.push_back(std::move(entry));
}

bool JobRegistrar::RemoveJob(
    absl::string_view name, absl::string_view ip_addr, absl::string_view port) {
  std::scoped_lock lock(mutex_);
  for (size_t i = 0; i < registry_.size(); i++) {
    if (registry_[i].name == name && registry_[i].ip_addr == ip_addr && registry_[i].port == port) {
      registry_[i] = registry_.back();
      registry_.pop_back();
      return true;
    }
  }
  return false;
}

std::vector<JobRegistrar::Entry> JobRegistrar::LookupJobsWithName(
    absl::string_view name) {
  std::scoped_lock lock(mutex_);
  std::vector<JobRegistrar::Entry> jobs;
  for (size_t i = 0; i < registry_.size(); i++) {
    if (registry_[i].name == name) {
      jobs.push_back(registry_[i]);
    }
  }
  return jobs;
}

std::vector<JobRegistrar::Entry>JobRegistrar::LookupJobsWithIpAddr(
    absl::string_view ip_addr) {
  std::scoped_lock lock(mutex_);
  std::vector<JobRegistrar::Entry> jobs;
  for (size_t i = 0; i < registry_.size(); i++) {
    if (registry_[i].ip_addr == ip_addr) {
      jobs.push_back(registry_[i]);
    }
  }
  return jobs;
}
