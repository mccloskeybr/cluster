load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
  name = "job_registrar",
  hdrs = ["job_registrar.h"],
  srcs = ["job_registrar.cc"],
  deps = [
    "@abseil-cpp//absl/log:log",
    "@abseil-cpp//absl/strings:string_view",
    "@abseil-cpp//absl/strings:strings",
    "@abseil-cpp//absl/log:check",
    "@grpc//:grpc++",
  ],
)

cc_library(
  name = "node_registrar",
  hdrs = ["node_registrar.h"],
  srcs = ["node_registrar.cc"],
  linkopts = ["-latomic"],
  deps = [
    ":network",
    ":resource_stats",
    "//src/node:config_cc_proto",
    "//src/node/client:node_service_client",
    "//src/node:node_service_cc_grpc",
    "@abseil-cpp//absl/log:check",
    "@abseil-cpp//absl/log:log",
    "@grpc//:grpc++",
  ],
)

cc_library(
  name = "resource_stats",
  hdrs = ["resource_stats.h"],
  srcs = ["resource_stats.cc"],
  deps = [
    "//src/node:node_service_cc_grpc",
    "@abseil-cpp//absl/log:check",
    "@abseil-cpp//absl/log:log",
    "@abseil-cpp//absl/strings:strings",
  ],
)

cc_library(
  name = "network",
  hdrs = ["network.h"],
  srcs = ["network.cc"],
  deps = [
    "@abseil-cpp//absl/log:check",
    "@grpc//:grpc++",
  ],
)
