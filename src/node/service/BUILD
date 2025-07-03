load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
  name = "node_service_impl",
  hdrs = ["node_service_impl.h"],
  srcs = [
    "node_service_impl.cc",
    "do_work_handler.cc",
    "get_usage_report_handler.cc",
    "poll_jobs_handler.cc",
    "resolve_name_handler.cc",
    "schedule_work_handler.cc",
  ],
  deps = [
    "//src/node/client:node_service_client",
    "//src/node:config_cc_proto",
    "//src/node:node_service_cc_grpc",
    "//src/node/service/util:job_registrar",
    "//src/node/service/util:network",
    "//src/node/service/util:usage_stats",
    "@abseil-cpp//absl/log:log",
    "@abseil-cpp//absl/strings:strings",
    "@abseil-cpp//absl/strings:string_view",
    "@grpc//:grpc++",
  ],
)
