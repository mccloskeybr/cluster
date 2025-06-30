load("@com_google_protobuf//bazel:cc_proto_library.bzl", "cc_proto_library")
load("@grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

package(default_visibility = ["//visibility:public"])

proto_library(
  name = "leader_service_proto",
  srcs = ["leader_service.proto"],
  deps = [],
)

cc_proto_library(
  name = "leader_service_cc_proto",
  deps = [":leader_service_proto"],
)

cc_grpc_library(
  name = "leader_service_cc_grpc",
  srcs = [":leader_service_proto"],
  grpc_only = True,
  deps = [":leader_service_cc_proto"],
)

proto_library(
  name = "config_proto",
  srcs = ["config.proto"],
  deps = [],
)

cc_proto_library(
  name = "config_cc_proto",
  deps = [":config_proto"],
)
