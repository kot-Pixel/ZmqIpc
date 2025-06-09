#include "ZSocketIpcDispatcher.hpp"

static IpcDispatcher dispatcher;
static std::mutex dispatcher_mutex;

bool ipc_register(const std::string& method, IpcHandler handler) {
    std::lock_guard<std::mutex> lock(dispatcher_mutex);
    return dispatcher.register_method(method, std::move(handler));
}

extern "C" bool ipc_dispatch(const char* method,
                             const void* req_buf, int req_len,
                             void* resp_buf, int* resp_len, int max_resp_len)
{
    if (!method || !req_buf || !resp_buf || !resp_len) return false;

    std::string req_payload((const char*)req_buf, req_len);
    std::string resp_payload;

    bool ok = dispatcher.dispatch(method, req_payload, resp_payload);
    if (!ok) return false;

    if ((int)resp_payload.size() > max_resp_len) return false;

    memcpy(resp_buf, resp_payload.data(), resp_payload.size());
    *resp_len = (int)resp_payload.size();

    return true;
}