#ifndef __DEMOROOTCONTEXT_H__
#define __DEMOROOTCONTEXT_H__
#include <string>
#include <string_view>
#include <unordered_map>

#include "proxy_wasm_intrinsics.h"

#include "google/protobuf/util/json_util.h"

#include "./generated/demo.pb.h"

using demoservice::Config;

class DemoRootContext : public RootContext {
public:
  explicit DemoRootContext(uint32_t id, std::string_view root_id) : RootContext(id, root_id) {}

  bool onConfigure(size_t) override;

  std::string_view getService();
  uint32_t getTimeout();

private:
  std::string service;
  demoservice::Config config;
};
#endif