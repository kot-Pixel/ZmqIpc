#include "NngUdsRpcPeerDial.hpp"

bool NngUdsRpcPeerDial::start()
{
    int rv;

    if ((rv = nng_pair0_open(&sock)) != 0)
    {
        LOGE("nng_pull_open: %s", nng_strerror(rv));
        return false;
    }

    if ((rv = nng_dial(sock, address.c_str(), nullptr, 0)) != 0)
    {
        LOGE("nng_dial: %s", nng_strerror(rv));
        nng_close(sock);
        return false;
    }

    for (int i = 0; i < num_workers; ++i)
    {
        auto *ctx = new NngPeerDialAioWithContext();
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

    LOGD("NNG dial to address at: %s", address.c_str());
    return true;
    return false;
}

void NngUdsRpcPeerDial::run_forever()
{
    while (true)
    {
        nng_msleep(1000);
    }
}

void NngUdsRpcPeerDial::stop()
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

void NngUdsRpcPeerDial::dispatch(const std::string &method_name, const void *req_buf, size_t len, std::string &resp_out)
{
    auto it = method_map.find(method_name);
    if (it == method_map.end())
    {
        LOGE("No such method registered: %s", method_name.c_str());
        return;
    }

    HandlerFunc &handler = it->second;
    handler(req_buf, len, resp_out);
}

void NngUdsRpcPeerDial::recv_cb(nng_aio *aio)
{
    int rv = nng_aio_result(aio);
    if (rv == 0)
    {
        nng_msg *msg = nng_aio_get_msg(aio);
        void *data = nng_msg_body(msg);
        size_t len = nng_msg_len(msg);

        flatbuffers::Verifier verifier(reinterpret_cast<const uint8_t *>(data), len);
        if (!verifier.VerifyBuffer<rpc::RpcMessage>())
        {
            LOGE("Failed to verify RpcMessage");
            return;
        }

        const rpc::RpcMessage *rpc_msg = flatbuffers::GetRoot<rpc::RpcMessage>(data);

        LOGD("Received RPC id=%llu, type=%d, method=%s, payload_size=%u",
             static_cast<unsigned long long>(rpc_msg->id()),
             static_cast<int>(rpc_msg->type()),
             rpc_msg->method() ? rpc_msg->method()->c_str() : "null",
             rpc_msg->payload() ? rpc_msg->payload()->size() : 0);

        switch (rpc_msg->type())
        {
        case rpc::RpcMessageType_REQUEST:
        {
            const std::string method_name = rpc_msg->method()->str();
            auto it = method_map.find(method_name);
            if (it != method_map.end())
            {
                const uint8_t *req_data = rpc_msg->payload()->data();
                size_t req_len = rpc_msg->payload()->size();

                std::string resp_bin;
                it->second(req_data, req_len, resp_bin);

                flatbuffers::FlatBufferBuilder builder;
                auto payload_offset = builder.CreateVector(
                    reinterpret_cast<const uint8_t *>(resp_bin.data()), resp_bin.size());

                auto method_offset = builder.CreateString(method_name);

                auto resp_offset = rpc::CreateRpcMessage(
                    builder,
                    rpc::RpcMessageType_RESPONSE,
                    rpc_msg->id(),
                    method_offset,
                    payload_offset);
                builder.Finish(resp_offset);

                nng_msg *nng_rsp_msg;
                nng_msg_alloc(&nng_rsp_msg, builder.GetSize());
                memcpy(nng_msg_body(nng_rsp_msg), builder.GetBufferPointer(), builder.GetSize());
                nng_sendmsg(sock, nng_rsp_msg, 0);
            }
            break;
        }
        case rpc::RpcMessageType_RESPONSE:
        {
            uint32_t resp_id = rpc_msg->id();

            std::shared_ptr<std::vector<uint8_t>> result;

            if (rpc_msg->payload())
            {
                auto payload = rpc_msg->payload();
                result = std::make_shared<std::vector<uint8_t>>(payload->begin(), payload->end());
            }

            {
                std::lock_guard<std::mutex> lock(pending_mutex);
                auto it = pending_requests.find(resp_id);
                if (it != pending_requests.end())
                {
                    it->second.set_value(result);
                    pending_requests.erase(it);
                }
                else
                {
                    LOGE("No pending request found for id=%u", resp_id);
                }
            }
            break;
        }

        default:
            break;
        }

        nng_msg_free(msg);
    }
    else
    {
        LOGE("Error receiving message: %s", nng_strerror(rv));
    }

    nng_recv_aio(sock, aio);
}

uint32_t NngUdsRpcPeerDial::generate_request_id()
{
    return next_request_id.fetch_add(1, std::memory_order_relaxed);
}

bool NngUdsRpcPeerDial::call_remote(const std::string &method,
                                    const void *req_buf, size_t len,
                                    uint8_t* &resp_buf, size_t* resp_size)
{
    uint32_t id = generate_request_id();

    flatbuffers::FlatBufferBuilder builder;
    auto method_offset = builder.CreateString(method);
    auto payload_offset = builder.CreateVector(reinterpret_cast<const uint8_t *>(req_buf), len);
    auto rpc_offset = rpc::CreateRpcMessage(builder, rpc::RpcMessageType_REQUEST, id, method_offset, payload_offset);
    builder.Finish(rpc_offset);

    nng_msg *msg;
    if (nng_msg_alloc(&msg, builder.GetSize()) != 0)
        return false;

    memcpy(nng_msg_body(msg), builder.GetBufferPointer(), builder.GetSize());

    std::promise<std::shared_ptr<std::vector<uint8_t>>> prom;
    std::future<std::shared_ptr<std::vector<uint8_t>>> fut = prom.get_future();

    {
        std::lock_guard<std::mutex> lock(pending_mutex);
        pending_requests[id] = std::move(prom);
    }

    if (nng_sendmsg(sock, msg, 0) != 0)
    {
        std::lock_guard<std::mutex> lock(pending_mutex);
        pending_requests.erase(id);
        return false;
    }

    if (fut.wait_for(std::chrono::seconds(3)) != std::future_status::ready)
    {
        std::lock_guard<std::mutex> lock(pending_mutex);
        pending_requests.erase(id);
        return false;
    }

    auto data_ptr = fut.get(); // shared_ptr<vector<uint8_t>>

    if (!data_ptr || data_ptr->empty())
        return false;

    resp_buf = new uint8_t[data_ptr->size()];
    memcpy(resp_buf, data_ptr->data(), data_ptr->size());
    if (resp_size)
        *resp_size = data_ptr->size();

    return true;
}
