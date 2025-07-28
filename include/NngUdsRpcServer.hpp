#include <nng/nng.h>
#include <string.h>
#include <vector>
#include <string>

#include <nng/protocol/pipeline0/pull.h>
#include <android/log.h>

#include "flatbuffers/verifier.h"
#include "rpc_generated.h"

#define PROJECT_TAG "Rpc"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, PROJECT_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, PROJECT_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, PROJECT_TAG, __VA_ARGS__)

// 包装结构体：绑定 AIO 与 this 指针
struct AioWithContext {
    nng_aio* aio = nullptr;
    class NngUdsRpcServer* server = nullptr;
};

class NngUdsRpcServer {
public:
    explicit NngUdsRpcServer(const std::string& addr = "abstract://nng.rpc", int workers = 16)
            : address(addr), num_workers(workers), sock{} {}

    ~NngUdsRpcServer() {
        stop();
    }

    bool start() {
        int rv;

        if ((rv = nng_pull_open(&sock)) != 0) {
            LOGE("nng_pull_open: %s", nng_strerror(rv));
            return false;
        }

        if ((rv = nng_listen(sock, address.c_str(), nullptr, 0)) != 0) {
            LOGE("nng_listen: %s", nng_strerror(rv));
            nng_close(sock);
            return false;
        }

        for (int i = 0; i < num_workers; ++i) {
            auto* ctx = new AioWithContext();
            ctx->server = this;

            rv = nng_aio_alloc(&ctx->aio, recv_cb_static, ctx);
            if (rv != 0) {
                LOGE("nng_aio_alloc: %s", nng_strerror(rv));
                delete ctx;
                return false;
            }

            aio_list.push_back(ctx);
            nng_recv_aio(sock, ctx->aio);
        }

        LOGD("NNG RPC server started at: %s", address.c_str());
        return true;
    }

    void run_forever() {
        while (true) {
            nng_msleep(1000);
        }
    }

    void stop() {
        for (auto* ctx : aio_list) {
            if (ctx && ctx->aio) {
                nng_aio_stop(ctx->aio);
                nng_aio_free(ctx->aio);
            }
            delete ctx;
        }
        aio_list.clear();
        nng_close(sock);
        LOGD("NNG RPC server stopped.");
    }

private:
    static void recv_cb_static(void* arg) {
        auto* ctx = static_cast<AioWithContext*>(arg);
        if (ctx && ctx->server) {
            ctx->server->recv_cb(ctx->aio);
        }
    }

    void recv_cb(nng_aio* aio) {
        int rv = nng_aio_result(aio);
        if (rv == 0) {
            nng_msg* msg = nng_aio_get_msg(aio);
            void* data = nng_msg_body(msg);
            size_t len = nng_msg_len(msg);

            flatbuffers::Verifier verifier(reinterpret_cast<const uint8_t*>(data), len);
            if (!rpc::VerifyRequestBuffer(verifier)) {
                LOGE("Invalid FlatBuffer message received.");
                nng_msg_free(msg);
                nng_recv_aio(sock, aio);
                return;
            }

            const rpc::Request* req = rpc::GetRequest(data);
            LOGD("Received RPC id=%llu, method=%s, payload_size=%zu",
                 static_cast<unsigned long long>(req->id()),
                 req->method() ? req->method()->c_str() : "null",
                 req->payload() ? req->payload()->size() : 0);

            // 处理请求后释放消息
            nng_msg_free(msg);
        } else {
            LOGE("Error receiving message: %s", nng_strerror(rv));
        }

        // 继续等待下一条消息
        nng_recv_aio(sock, aio);
    }

private:
    std::string address;
    int num_workers;
    nng_socket sock;
    std::vector<AioWithContext*> aio_list;
};
