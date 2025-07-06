#include "src/node/service/node_service_impl.h"

#include <filesystem>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <system_error>
#include <thread>

#include "absl/log/log.h"
#include "absl/strings/string_view.h"
#include "absl/strings/str_cat.h"
#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/util/network.h"

namespace {

void ExecuteJob(JobRegistrar* job_registrar, std::string job_name) {
  const JobMetadata job = job_registrar->GetJob(job_name);
  const std::string exec_call = absl::StrCat(job.path, " ", job.args);
  LOG(INFO) << "Executing job: " << job_name << ": " << exec_call;
  job_registrar->UpdateJobState(job.name, JobState::IN_PROGRESS);
  const int32_t status = std::system(exec_call.c_str());
  LOG(INFO) << "Job: " << job_name << " completed with status: " << status;
  job_registrar->UpdateJobState(
      job.name, status == 0 ? JobState::COMPLETE_SUCCESS : JobState::COMPLETE_ERROR);
}

} // namespace

grpc::Status NodeServiceImpl::DoWork(
    grpc::ServerContext* context,
    const proto::DoWorkRequest* request,
    proto::DoWorkResponse* response) {
  LOG(INFO) << "Attempting to schedule: " << request->job_name() << " for execution.";
  if (request->job_name().empty()) {
    return grpc::Status(
        grpc::StatusCode::INVALID_ARGUMENT,
        "job_name cannot be empty.");
  }

  // NOTE: successful registry is a blessing to clobber any existing files.
  // We assume that if there are any jobs using that binary registration fails.
  const grpc::Status status = job_registrar_.RegisterJob(request->job_name());

  const static std::string kWorkDir = absl::StrCat(std::getenv("HOME"), "/work/");
  std::filesystem::create_directory(kWorkDir);
  const std::string exec_file = absl::StrCat(kWorkDir, request->job_name());
  {
    std::ofstream file(exec_file, std::ios::binary);
    if (!file.is_open()) {
      job_registrar_.UpdateJobState(request->job_name(), JobState::COMPLETE_ERROR);
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          absl::StrCat("Unable to open executable: ", exec_file));
    }
    file << request->executable();
    if (file.fail()) {
      job_registrar_.UpdateJobState(request->job_name(), JobState::COMPLETE_ERROR);
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          absl::StrCat("Unable to write executable: ", exec_file));
    }
  }
  std::error_code error;
  std::filesystem::permissions(
      exec_file, std::filesystem::perms::owner_exec,
      std::filesystem::perm_options::add, error);
  if (error) {
    job_registrar_.UpdateJobState(request->job_name(), JobState::COMPLETE_ERROR);
    return grpc::Status(
        grpc::StatusCode::INTERNAL,
        absl::StrCat("Unable to update permissions: ", exec_file));
  }

  // TODO: likely want to keep track of thread to support cancellation.
  job_registrar_.FinalizeJob(request->job_name(), request->port(), request->args(), exec_file);
  std::thread work_thread(ExecuteJob, &job_registrar_, request->job_name());
  work_thread.detach();

  return grpc::Status::OK;
}
