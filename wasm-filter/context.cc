#include "./context.h"
#include "./rootcontext.h"
#include "./generated/demo.pb.h"
#include "./grpc.h"

using demoservice::Config;
using demoservice::DemoReply;
using demoservice::DemoRequest;

FilterHeadersStatus DemoContext::onRequestHeaders(uint32_t, bool end_of_stream)
{
  LOG_TRACE(std::string("onRequestHeaders called ") + std::to_string(id())); 
  LOG_INFO("Sending headers");
  auto res = initRequest();
  if (res != WasmResult::Ok)
  {
    LOG_ERROR("onRequestHeaders failed: " + toString(res));
    closeRequest();
    return FilterHeadersStatus::StopIteration;
  }

  // Send the headers
  auto request = headerRequest(end_of_stream);
  res = requestHandler->send(*request, end_of_stream);
  if (res != WasmResult::Ok)
  {
    LOG_ERROR("onRequestHeaders send failed: " + toString(res));
    closeRequest();
    return FilterHeadersStatus::StopIteration;
  }

  // StopIteration is converted to StopAllIterationAndWatermark in envoy. Returning either hangs
  // the execution and onRequestBody is never called. If Continue is returned then the grpc OnReceive 
  // cannot set the headers.
  // Quote from envoy source:
  // "Do not iterate for headers as well as data and trailers for the current filter and the filters following, and buffer body data for later dispatching."
  // So how do I trigger onRequestBody for this filter and then continue the filter chain.
  return FilterHeadersStatus::StopAllIterationAndWatermark;
}

FilterDataStatus DemoContext::onRequestBody(size_t body_buffer_length, bool end_of_stream)
{
  LOG_INFO("onRequestBody");

  LOG_INFO("Sending body");
  auto request = bodyRequest(body_buffer_length, end_of_stream);
  auto res = requestHandler->send(*request, end_of_stream);
  if (res != WasmResult::Ok)
  {
    LOG_ERROR("onRequestHeaders failed: " + toString(res));
    closeRequest();
    return FilterDataStatus::StopIterationNoBuffer;
  }

  return FilterDataStatus::StopIterationAndBuffer;
}

/*
  Initialize a request handler.
 */
WasmResult DemoContext::initRequest()
{
  DemoRootContext *demoRootContext = dynamic_cast<DemoRootContext *>(root());

  // Start a new invocation
  HeaderStringPairs metadata;
  requestHandler = new InvokeDemoStreamHandler(this);

  auto res = demoRootContext->grpcStreamHandler(
      demoRootContext->getService(), ServiceName, InvokeDemoMethodName,
      metadata, std::unique_ptr<GrpcStreamHandlerBase>(requestHandler));

  return res;
}

std::unique_ptr<DemoRequest> DemoContext::headerRequest(bool eos)
{
  std::unique_ptr<DemoRequest> request(new DemoRequest());

  request->set_endofstream(eos);

  auto reqheaders = getRequestHeaderPairs();
  auto headers = reqheaders->pairs();
  for (auto &hdr : headers)
  {
    std::string key = std::string(hdr.first);
    std::string value = std::string(hdr.second);
    LOG_INFO(key + std::string("->") + value);
    (*request->mutable_headers())[key] = value;

    if (request->id().size() == 0 && key.compare("x-request-id") == 0)
    {
      request->set_id(value);
      LOG_INFO(std::string("REQUEST ID -> ") + value);
    }
  }

  return request;
}

std::unique_ptr<DemoRequest> DemoContext::bodyRequest(size_t body_buffer_length, bool eos)
{
  auto body = getBufferBytes(WasmBufferType::HttpRequestBody, 0, body_buffer_length);
  std::unique_ptr<DemoRequest> request(new DemoRequest());
  request->set_body(body->data());
  request->set_endofstream(eos);
  return request;
}
