#include "NngUdsRpcPullListener.hpp"
#include <nng/protocol/pipeline0/pull.h>

NngUdsRpcPullListener::NngUdsRpcPullListener() {
    sock.id = 0;
}

NngUdsRpcPullListener::~NngUdsRpcPullListener() {
    close();
}

bool NngUdsRpcPullListener::init(const char* url) {
    if (nng_pull0_open(&sock) != 0) return false;
    nng_socket_set_int(sock, NNG_OPT_RECVBUF, 120);
    if (nng_listen(sock, url, NULL, 0) != 0) return false;
    return true;
}

void NngUdsRpcPullListener::startRecvLoop(RecvCallback cb) {
    if (running) {
        LOGE_PULL("recv looper is running, so return.....");
        return;
    }
    running = true;

    recvThread = std::thread([this, cb]() {
        LOGE_PULL("start recv looper");
        while (running) {
            char* buf = nullptr;
            size_t sz = 0;
            int rv = nng_recv(sock, &buf, &sz, NNG_FLAG_ALLOC);
            LOGE_PULL("startRecvLoop: revcData code %d", rv);
            LOGE_PULL("startRecvLoop: revcData str %s", nng_strerror(rv));
            if (rv == 0 && buf != nullptr) {
                cb(reinterpret_cast<const uint8_t*>(buf), sz);
                nng_free(buf, sz);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    });
}


void NngUdsRpcPullListener::close() {
    if (running) {
        running = false;
        if (recvThread.joinable()) {
            recvThread.join();
        }
    }
    if (sock.id != 0) {
        nng_close(sock);
        sock.id = 0;
    }
}
