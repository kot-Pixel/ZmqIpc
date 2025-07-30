#include "NngUdsRpcServer.hpp"

bool NngUdsRpcServer::start()
{
    int rv;

    if ((rv = nng_pull_open(&sock)) != 0)
    {
        LOGE("nng_pull_open: %s", nng_strerror(rv));
        return false;
    }

    if ((rv = nng_listen(sock, address.c_str(), nullptr, 0)) != 0)
    {
        LOGE("nng_listen: %s", nng_strerror(rv));
        nng_close(sock);
        return false;
    }

    for (int i = 0; i < num_workers; ++i)
    {
        auto *ctx = new AioWithContext();
        ctx->server = this;

        rv = nng_aio_alloc(&ctx->aio, recv_cb_static, ctx);
        if (rv != 0)
        {
            LOGE("nng_aio_alloc: %s", nng_strerror(rv));
            delete ctx;
            return false;
        }

        aio_list.push_back(ctx);
        nng_recv_aio(sock, ctx->aio);
    }

    LOGD("NNG RPC server started at: %s", address.c_str());
    return true;
    return false;
}

void NngUdsRpcServer::run_forever()
{
    while (true)
    {
        nng_msleep(1000);
    }
}

void NngUdsRpcServer::stop()
{
    for (auto *ctx : aio_list)
    {
        if (ctx && ctx->aio)
        {
            nng_aio_stop(ctx->aio);
            nng_aio_free(ctx->aio);
        }
        delete ctx;
    }
    aio_list.clear();
    nng_close(sock);
    LOGD("NNG RPC server stopped.");
}

void NngUdsRpcServer::dispatch(const std::string &method_name, const void *req_buf, size_t len, std::string &resp_out)
{
    auto it = method_map.find(method_name);
    if (it == method_map.end()) {
        LOGE("No such method registered: %s", method_name.c_str());
        return;
    }

    HandlerFunc& handler = it->second;
    handler(req_buf, len, resp_out);
}

void NngUdsRpcServer::recv_cb(nng_aio *aio)
{
    int rv = nng_aio_result(aio);
    if (rv == 0)
    {
        nng_msg *msg = nng_aio_get_msg(aio);
        void *data = nng_msg_body(msg);
        size_t len = nng_msg_len(msg);

        flatbuffers::Verifier verifier(reinterpret_cast<const uint8_t *>(data), len);
        if (!rpc::VerifyRequestBuffer(verifier))
        {
            LOGE("Invalid FlatBuffer message received.");
            nng_msg_free(msg);
            nng_recv_aio(sock, aio);
            return;
        }

        const rpc::Request *req = rpc::GetRequest(data);
        LOGD("Received RPC id=%llu, method=%s, payload_size=%u",
             static_cast<unsigned long long>(req->id()),
             req->method() ? req->method()->c_str() : "null",
             req->payload() ? req->payload()->size() : 0);

        const std::string method_name = req->method()->str();
        auto it = method_map.find(method_name);
        if (it != method_map.end()) {
            const uint8_t* req_data = req->payload()->data();
            size_t req_len = req->payload()->size();

            std::string resp_bin;
            it->second(req_data, req_len, resp_bin);

            flatbuffers::FlatBufferBuilder builder;
            auto payload_offset = builder.CreateVector(
                reinterpret_cast<const uint8_t*>(resp_bin.data()), resp_bin.size());

            auto resp_offset = rpc::CreateResponse(builder, req->id(), 200, payload_offset);
            builder.Finish(resp_offset);

            nng_msg* msg;
            nng_msg_alloc(&msg, builder.GetSize());
            memcpy(nng_msg_body(msg), builder.GetBufferPointer(), builder.GetSize());
            nng_sendmsg(sock, msg, 0);
        }

        nng_msg_free(msg);
    }
    else
    {
        LOGE("Error receiving message: %s", nng_strerror(rv));
    }

    nng_recv_aio(sock, aio);
}