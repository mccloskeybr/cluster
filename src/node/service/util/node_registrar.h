#ifndef SRC_NODE_SERVICE_UTIL_NODE_REGISTRAR_H_
#define SRC_NODE_SERVICE_UTIL_NODE_REGISTRAR_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "src/node/config.pb.h"
#include "src/node/client/node_service_client.h"
#include "src/node/service/util/resource_stats.h"

enum class NodeState {
  UNDEFINED,
  UNHEALTHY,
  HEALTHY,
};

// TODO: nodes can be referenced by multiple IPs.
// when a node is being registered, it should send an RPC, and get all
// possible RPCs that node may use in the response.
struct NodeMetadata {
  std::string ip_addr;
  std::string port;
  std::shared_ptr<NodeServiceClient> client;
  bool is_self;
  NodeState state;
  ResourceStats local_stats;
  ResourceStats aggregate_stats;
};

class NodeRegistrar {
 public:
  explicit NodeRegistrar(const proto::NodeConfig& config);
  ~NodeRegistrar();

  std::vector<NodeMetadata> GetSnapshot();
  ResourceStats GetAggregateStats();

 private:
  std::mutex mutex_;
  std::vector<NodeMetadata> nodes_;
  std::vector<std::thread> threads_;
  std::atomic<bool> stop_threads_;
  // NOTE: this may be a copy of one of the nodes aggregate stats.
  // It is separate to ensure there isn't a deadlock when recalculating available resources.
  std::atomic<ResourceStats> aggregate_stats_;

  friend void PollNodeStatus(NodeRegistrar*);
};

#endif // SRC_NODE_SERVICE_UTIL_NODE_REGISTRAR_H_
