#include "src/node/service/node_service_impl.h"

#include <chrono>
#include <cstdio>
#include <thread>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/usage_stats.h"

grpc::Status NodeServiceImpl::GetUsageReport(
    grpc::ServerContext* context,
    const node::GetUsageReportRequest* request,
    node::GetUsageReportResponse* response) {
  const auto prev_stats = UsageStats::Collect();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  const auto curr_stats = UsageStats::Collect();

  const uint64_t active = curr_stats.active - prev_stats.active;
  const uint64_t total = curr_stats.total - prev_stats.total;
  if (total == 0) {
    return grpc::Status(grpc::StatusCode::INTERNAL,
        absl::StrCat("Error calculating resource utilization! ",
          prev_stats.DebugString(), " ", curr_stats.DebugString()));
  }
  response->set_cpu_usage(((float) active) / total);

  return grpc::Status::OK;
}
