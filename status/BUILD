load("@com_google_protobuf//bazel:cc_proto_library.bzl", "cc_proto_library")
load("@grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

proto_library(
  name = "service_proto",
  srcs = ["service.proto"],
  deps = [],
)

cc_proto_library(
  name = "service_cc_proto",
  deps = [":service_proto"],
)

cc_grpc_library(
  name = "service_cc_grpc",
  srcs = [":service_proto"],
  grpc_only = True,
  deps = [":service_cc_proto"],
)

cc_binary(
  name = "main",
  srcs = ["main.cc"],
  deps = [
    ":service_cc_grpc",
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
