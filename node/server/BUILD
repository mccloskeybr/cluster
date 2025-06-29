load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
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
    "//node:node_service_cc_grpc",
    "@abseil-cpp//absl/strings:strings",
    "@abseil-cpp//absl/strings:string_view",
    "@grpc//:grpc++",
  ],
)

cc_binary(
  name = "server_main",
  srcs = ["server_main.cc"],
  args = [
    "--stderrthreshold=0",
  ],
  deps = [
    ":node_service_impl",
    "@abseil-cpp//absl/flags:flag",
    "@abseil-cpp//absl/flags:parse",
    "@abseil-cpp//absl/log:initialize",
    "@abseil-cpp//absl/log:log",
    "@abseil-cpp//absl/log:flags",
    "@abseil-cpp//absl/strings:str_format",
    "@grpc//:grpc++",
    "@grpc//:grpc++_reflection",
  ]
)

