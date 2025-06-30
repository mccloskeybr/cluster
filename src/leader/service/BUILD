load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
  name = "leader_service_impl",
  hdrs = ["leader_service_impl.h"],
  srcs = [
    "leader_service_impl.cc",
    "schedule_work_handler.cc",
  ],
  deps = [
    "//src/leader:leader_service_cc_grpc",
    "//src/leader:config_cc_proto",
    "//src/node/client:node_service_client",
    "@abseil-cpp//absl/log:log",
    "@abseil-cpp//absl/strings:strings",
    "@abseil-cpp//absl/strings:string_view",
    "@grpc//:grpc++",
  ],
)
