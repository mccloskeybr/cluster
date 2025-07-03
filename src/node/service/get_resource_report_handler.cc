#include "src/node/service/node_service_impl.h"

#include <chrono>
#include <cstdio>
#include <thread>
#include <unistd.h>
#include <grpcpp/grpcpp.h>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "src/node/node_service.grpc.pb.h"
#include "src/node/service/util/resource_stats.h"

grpc::Status NodeServiceImpl::GetResourceReport(
    grpc::ServerContext* context,
    const proto::GetResourceReportRequest* request,
    proto::GetResourceReportResponse* response) {
  static const auto kProbeTime = std::chrono::milliseconds(1000);
  const auto prev = ResourceStats::Collect();
  std::this_thread::sleep_for(kProbeTime);
  const auto curr = ResourceStats::Collect();

  const ResourceStats report =
    (curr - prev) / std::chrono::duration_cast<std::chrono::seconds>(kProbeTime).count();
  LOG(INFO) << "Calculated usage report for self - " << report.DebugString();
  if (report.total == 0) {
    return grpc::Status(
        grpc::StatusCode::INTERNAL,
        absl::StrCat("Error calculating resource utilization! ",
          report.DebugString()));
  }

  *response->mutable_local_stats() = ResourceStats::ToProto(report);
  *response->mutable_aggregate_stats() =
    ResourceStats::ToProto(node_registrar_.GetAggregateStats());

  return grpc::Status::OK;
}
