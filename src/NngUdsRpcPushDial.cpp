#include "NngUdsRpcPushDial.hpp"
#include <nng/protocol/pipeline0/push.h>

NngUdsRpcPushDial::NngUdsRpcPushDial() {
    sock.id = 0;
}

NngUdsRpcPushDial::~NngUdsRpcPushDial() {
    close();
}

bool NngUdsRpcPushDial::init(const std::string &url)
{
    if (nng_push0_open(&sock) != 0) return false;
    nng_socket_set_int(sock, NNG_OPT_SENDBUF, 120);
    if (nng_dial(sock, url.c_str(), NULL, 0) != 0) return false;
    return true;
}

bool NngUdsRpcPushDial::sendData(const uint8_t *data, size_t len)
{
    int rv = nng_send(sock, (void*)data, len, 0);
    LOGE("NngUdsRpcPushDial: sendData str %s", nng_strerror(rv));
    LOGE("NngUdsRpcPushDial: sendData code %d", rv);
    return rv == 0;
}

void NngUdsRpcPushDial::close() {
    if (sock.id != 0) {
        nng_close(sock);
        sock.id = 0;
    }
}