#include "src/node/service/util/job_registrar.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <grpcpp/grpcpp.h>

#include "absl/log/check.h"
#include "absl/strings/string_view.h"
#include "absl/strings/str_cat.h"

namespace {

bool JobStateIsInProgress(JobState state) {
  using enum JobState;
  switch (state) {
    case PENDING:
    case IN_PROGRESS:
      return true;
    default:
      return false;
  }
}

} // namespace

grpc::Status JobRegistrar::RegisterJob(absl::string_view name) {
  std::scoped_lock lock(mutex_);
  JobMetadata entry = {
    .name = std::string(name),
    .port = "",
    .args = "",
    .path = "",
    .state = JobState::PENDING,
  };
  for (size_t i = 0; i < registry_.size(); i++) {
    if (registry_[i].name == name) {
      if (JobStateIsInProgress(registry_[i].state)) {
        return grpc::Status(
            grpc::StatusCode::ABORTED,
            absl::StrCat("Job with name: ", name, " is already scheduled!"));
      } else {
        registry_[i] = std::move(entry);
        return grpc::Status::OK;
      }
    }
  }
  registry_.push_back(std::move(entry));
  return grpc::Status::OK;
}

void JobRegistrar::FinalizeJob(
    absl::string_view name, absl::string_view port,
    absl::string_view args, absl::string_view path) {
  std::scoped_lock lock(mutex_);
  for (size_t i = 0; i < registry_.size(); i++) {
    if (registry_[i].name == name) {
      registry_[i].port = std::string(port);
      registry_[i].args = std::string(args);
      registry_[i].path = std::string(path);
      return;
    }
  }
  CHECK(false) << "Unexpectedly unable to update path for not found job: " << name;
}

void JobRegistrar::UpdateJobState(absl::string_view name, JobState state) {
  std::scoped_lock lock(mutex_);
  for (size_t i = 0; i < registry_.size(); i++) {
    if (registry_[i].name == name) {
      registry_[i].state = state;
      return;
    }
  }
  CHECK(false) << "Unexpectedly unable to update state for not found job: " << name;
}

JobMetadata JobRegistrar::GetJob(absl::string_view name) {
  std::scoped_lock lock(mutex_);
  for (size_t i = 0; i < registry_.size(); i++) {
    if (registry_[i].name == name) {
      return registry_[i];
    }
  }
  CHECK(false) << "Unexpectedly unable to find job: " << name;
}

std::vector<JobMetadata> JobRegistrar::GetSnapshot() {
  std::scoped_lock lock(mutex_);
  return registry_;
}
