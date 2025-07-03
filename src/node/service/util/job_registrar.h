#ifndef SRC_NODE_SERVICE_UTIL_JOB_REGISTRAR_H_
#define SRC_NODE_SERVICE_UTIL_JOB_REGISTRAR_H_

#include <mutex>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>

#include "absl/strings/string_view.h"
#include "absl/strings/str_cat.h"

enum class JobState {
  UNDEFINED,
  PENDING,
  IN_PROGRESS,
  COMPLETE_SUCCESS,
  COMPLETE_ERROR,
};

// TODO: some of this can be moved to proto when there's a schema for that.
struct JobMetadata {
  std::string name;
  std::string port;
  std::string args;
  std::string path;
  JobState state;
};

// NOTE: tracks job scheduling within a single node only.
// This class is thread safe.
class JobRegistrar {
 public:
  explicit JobRegistrar()
    : mutex_(), registry_() {}

  grpc::Status RegisterJob(absl::string_view name);

  void FinalizeJob(absl::string_view name, absl::string_view port,
                   absl::string_view args, absl::string_view path);

  void UpdateJobState(absl::string_view name, JobState state);

  JobMetadata GetJob(absl::string_view name);

  std::vector<JobMetadata> GetSnapshot();

 private:
  std::mutex mutex_;
  std::vector<JobMetadata> registry_;
};

#endif
