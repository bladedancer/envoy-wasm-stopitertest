#include "./grpc.h"

void InvokeDemoStreamHandler::onRemoteClose(GrpcStatus status)
{
    LOG_INFO("GRPC stream :: onRemoteClose");
}

void InvokeDemoStreamHandler::onReceive(size_t body_size)
{
    democontext->setEffectiveContext();

    LOG_INFO(std::string("GRPC stream :: onReceive ") + std::to_string(body_size));
    WasmDataPtr response_data = getBufferBytes(WasmBufferType::GrpcReceiveBuffer, 0, body_size);
    const DemoReply &response = response_data->proto<DemoReply>();

    LOG_INFO(std::string("GRPC stream :: header count ") + std::to_string(response.headers_size()));
    LOG_INFO(std::string("GRPC stream :: body size ") + std::to_string(response.body().size()));

    if (response.headers_size() > 0) {
        auto res = updateHeaders(response);
        if (res != WasmResult::Ok)
        {
            LOG_INFO(std::string("updateheaders :: failed ") + toString(res));
            closeRequest();
            return;
        }
    }
    
    if (response.body().size() > 0) {
        auto resBody = updateBody(response);
        if (resBody != WasmResult::Ok)
        {
            LOG_INFO(std::string("updateBody :: failed ") + toString(resBody));
            closeRequest();
            return;
        }
    }

    if (response.endofstream())
    {
        continueRequest();
    }
}

WasmResult InvokeDemoStreamHandler::updateHeaders(const DemoReply &response)
{
    LOG_INFO(std::string("updateheaders"));

    if (response.headers_size())
    {
        HeaderStringPairs headers;
        for (auto hdr : response.headers())
        {
            LOG_INFO(hdr.first + " = " + hdr.second);
            headers.push_back(std::make_pair(std::string(hdr.first), std::string(hdr.second)));
        }
        auto res = setRequestHeaderPairs(headers);
        if (res != WasmResult::Ok)
        {
            LOG_INFO(std::string("setRequestHeaderPairs: failed") + toString(res));
            return res;
        }

        // DEBUG
        auto reqheaders = getRequestHeaderPairs();
        auto pairs = reqheaders->pairs();
        LOG_INFO(std::string("headers: ") + std::to_string(pairs.size()));
        for (auto &p : pairs)
        {
            LOG_INFO(std::string(p.first) + std::string(" -> ") + std::string(p.second));
        }
    }

    return WasmResult::Ok;
}

WasmResult InvokeDemoStreamHandler::updateBody(const DemoReply &response)
{
    LOG_INFO(std::string("updateBody"));
    if (response.body().size())
    {
        return setBuffer(WasmBufferType::HttpRequestBody, 0, response.body().size(), response.body());
    }
    return WasmResult::Ok;
}