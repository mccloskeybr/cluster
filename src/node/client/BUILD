load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
  name = "node_service_client",
  hdrs = ["node_service_client.h"],
  srcs = ["node_service_client.cc"],
  deps = [
    "//src/node:node_service_cc_grpc",
    "@abseil-cpp//absl/log:log",
    "@grpc//:grpc++",
  ],
)
