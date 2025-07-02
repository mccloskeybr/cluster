#include "src/node/service/node_service_impl.h"

#include <filesystem>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"

namespace {

std::vector<std::string> GetAllFilenamesInDirectory(absl::string_view dir_path) {
  std::vector<std::string> filenames;
  for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
    if (std::filesystem::is_regular_file(entry.status())) {
      filenames.push_back(entry.path().filename().string());
    }
  }
  return filenames;
}

} // namespace

grpc::Status NodeServiceImpl::PollJobs(
    grpc::ServerContext* context,
    const node::PollJobsRequest* request,
    node::PollJobsResponse* response) {
  const static std::string kWorkDir = absl::StrCat(std::getenv("HOME"), "/work/");
  std::vector<std::string> filenames = GetAllFilenamesInDirectory(kWorkDir);
  response->mutable_jobs()->Add(filenames.begin(), filenames.end());
  return grpc::Status::OK;
}
