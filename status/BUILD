load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@com_google_protobuf//bazel:cc_proto_library.bzl", "cc_proto_library")

package(default_visibility = ["//visibility:public"])

proto_library(
  name = "service_proto",
  srcs = ["service.proto"],
  deps = [],
)

cc_proto_library(
  name = "service_cc_proto",
  deps = [":service_proto"],
)

cc_binary(
  name = "main",
  srcs = ["main.cc"],
  deps = [
    ":service_cc_proto",
  ]
)
