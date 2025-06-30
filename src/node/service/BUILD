load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
  name = "usage_stats",
  hdrs = ["usage_stats.h"],
  srcs = ["usage_stats.cc"],
  deps = [
    "@abseil-cpp//absl/strings:strings",
  ],
)

cc_library(
  name = "node_service_impl",
  hdrs = ["node_service_impl.h"],
  srcs = [
    "do_work_handler.cc",
    "get_usage_report_handler.cc"
  ],
  deps = [
    ":usage_stats",
    "//src/node:node_service_cc_grpc",
    "@abseil-cpp//absl/log:log",
    "@abseil-cpp//absl/strings:strings",
    "@abseil-cpp//absl/strings:string_view",
    "@grpc//:grpc++",
  ],
)
