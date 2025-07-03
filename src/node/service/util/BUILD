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
  name = "usage_stats",
  hdrs = ["usage_stats.h"],
  srcs = ["usage_stats.cc"],
  deps = [
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
