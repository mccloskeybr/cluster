#include "node/server/node_service_impl.h"

#include <filesystem>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <system_error>
#include <thread>

#include "absl/log/log.h"
#include "absl/strings/string_view.h"
#include "absl/strings/str_cat.h"
#include "node/node_service.grpc.pb.h"

namespace node {
namespace {

void TryRemoveFile(absl::string_view file_path) {
  std::error_code error;
  if (!std::filesystem::remove(file_path, error)) {
    LOG(ERROR) << "Error removing file: " << file_path << ": " << error.message();
  }
}

} // namespace

grpc::Status NodeServiceImpl::DoWork(
    grpc::ServerContext* context,
    const node::DoWorkRequest* request,
    node::DoWorkResponse* response) {
  LOG(INFO) << "Received request to execute: " << request->job_name();
  if (request->job_name().empty()) {
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "job_name cannot be empty.");
  }

  const static std::string kWorkDir = absl::StrCat(std::getenv("HOME"), "/work/");
  std::filesystem::create_directory(kWorkDir);

  const std::string exec_file = absl::StrCat(kWorkDir, request->job_name());
  if (std::filesystem::exists(exec_file)) {
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
        absl::StrCat("Job with name: ", request->job_name(), " is already scheduled."));
  }

  {
    std::ofstream file(exec_file, std::ios::binary);
    if (!file.is_open()) {
      return grpc::Status(grpc::StatusCode::INTERNAL,
          absl::StrCat("Unable to open executable for job name: ", request->job_name()));
    }
    file << request->executable();
    if (file.fail()) {
      return grpc::Status(grpc::StatusCode::INTERNAL,
          absl::StrCat("Unable to copy executable for job name: ", request->job_name()));
    }
  }

  std::error_code error;
  std::filesystem::permissions(exec_file, std::filesystem::perms::owner_exec, error);
  if (error) { TryRemoveFile(exec_file); }

  LOG(INFO) << "Scheduling: " << request->job_name() << " for execution.";
  std::thread async_work_thread(
      [](std::string exec_file, std::string args) {
        LOG(INFO) << "Executing: " << exec_file;
        std::system(absl::StrCat(exec_file, " ", args).c_str());
        TryRemoveFile(exec_file);
      }, exec_file, request->args());
  async_work_thread.detach();

  return grpc::Status::OK;
}

} // namespace node
