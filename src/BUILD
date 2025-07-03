load("@rules_cc//cc:cc_binary.bzl", "cc_binary")

cc_binary(
  name = "main",
  srcs = ["main.cc"],
  args = [
    "--stderrthreshold=0",
    "--port=6000",
    "--config_path=/home/mccloskeybr/node_config.txtpb",
  ],
  deps = [
    "//src/node/service:node_service_impl",
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
