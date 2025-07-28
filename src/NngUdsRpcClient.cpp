#include "NngUdsRpcClient.hpp"

#include "rpc_generated.h"
#include <flatbuffers/flatbuffers.h>
#include <iostream>

#include "nng/protocol/pipeline0/push.h"

NngUdsRpcClient::NngUdsRpcClient(const std::string& url)
    : m_url(url), m_connected(false) {
    if (nng_push0_open(&m_sock) != 0) {
        Client_LOGD("nng_req0_open failed");
    }
}

NngUdsRpcClient::~NngUdsRpcClient() {
    if (m_connected) {
        nng_close(m_sock);
    }
}

bool NngUdsRpcClient::connect() {
    if (nng_dial(m_sock, m_url.c_str(), nullptr, 0) != 0) {
        Client_LOGD("nng_dial failed");
        return false;
    }

    Client_LOGD("nng_dial success");

    m_connected = true;
    return true;
}

std::string NngUdsRpcClient::call(const std::string& method, const std::string& param) {
    if (!m_connected) {
        Client_LOGD("Not connected.");
        return "";
    }

    flatbuffers::FlatBufferBuilder builder;
    auto method_str = builder.CreateString(method);
    auto payload_vec = builder.CreateVector(
        reinterpret_cast<const uint8_t*>(param.data()), param.size());
    auto req = rpc::CreateRequest(builder, 1, method_str, payload_vec);
    builder.Finish(req);

    
    Client_LOGD("rpc CreateRequest success");

    nng_msg* msg;
    nng_msg_alloc(&msg, 0);
    nng_msg_append(msg, builder.GetBufferPointer(), builder.GetSize());

    if (nng_sendmsg(m_sock, msg, 0) != 0) {
        Client_LOGD("send failed");
        nng_msg_free(msg);
        return "";
    }

    Client_LOGD("rpc nng_sendmsg success");

    if (nng_recvmsg(m_sock, &msg, 0) != 0) {
       Client_LOGD("recv failed\n");
        return "";
    }

    // // 解析响应
    // auto resp = rpc::GetResponse(nng_msg_body(msg));
    // std::string result(reinterpret_cast<const char*>(resp->result()->data()),
    //                    resp->result()->size());

    nng_msg_free(msg);
    return "";
}
