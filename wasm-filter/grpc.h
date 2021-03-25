#ifndef __DEMOGRPC_H__
#define __DEMOGRPC_H__

#include "proxy_wasm_intrinsics.h"
#include "proxy_wasm_intrinsics.pb.h"
#include "./generated/demo.pb.h"
#include "./context.h"

using demoservice::DemoReply;
using demoservice::DemoRequest;

static constexpr char ServiceName[] = "demoservice.DemoServer";
static constexpr char InvokeDemoMethodName[] = "InvokeDemo";

class InvokeDemoStreamHandler : public GrpcStreamHandler<DemoRequest, DemoReply>
{
public:
  InvokeDemoStreamHandler(DemoContext *dc) : democontext(dc){};
  void onReceive(size_t body_size) override;
  void onRemoteClose(GrpcStatus status) override;

private:
  WasmResult updateHeaders(const DemoReply &response);
  WasmResult updateBody(const DemoReply &response);
  DemoContext *democontext;
};

#endif