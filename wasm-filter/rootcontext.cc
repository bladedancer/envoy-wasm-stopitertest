#include "./rootcontext.h"
#include "proxy_wasm_intrinsics.pb.h"
#include "google/protobuf/util/json_util.h"

using google::protobuf::util::JsonParseOptions;
using google::protobuf::util::error::Code;
using google::protobuf::util::Status;


std::string_view DemoRootContext::getService()
{
    return service;
}

uint32_t DemoRootContext::getTimeout() {
    return config.timeout();
}

bool DemoRootContext::onConfigure(size_t config_size)
{
    LOG_TRACE("onConfigure called");
    const WasmDataPtr configuration = getBufferBytes(WasmBufferType::PluginConfiguration, 0, config_size);

    JsonParseOptions jsonOptions;
    const Status options_status = JsonStringToMessage(
        configuration->toString(),
        &config,
        jsonOptions);
    if (options_status != Status::OK)
    {
        LOG_ERROR("Cannot parse plugin configuration JSON string: " + configuration->toString());
        return false;
    }
    LOG_INFO("Loading Config: " + config.clustername());

    // Generate the GRPC configuration
    GrpcService grpc_service;
    grpc_service.mutable_envoy_grpc()->set_cluster_name(config.clustername());
    grpc_service.SerializeToString(&service);

    return true;
}
