#include "ZSocketIpcClient.hpp"

void initClientSocketIpcEnv() {
    zSocketIpcClientCxt = zmq_ctx_new();
    zSocketClient = zmq_socket(zSocketIpcClientCxt, ZMQ_DEALER);

    printf("initClientSocketIpcEnv invoke complete\n");
}

void connectServer() {
    zmq_setsockopt(zSocketClient, ZMQ_IDENTITY, "wwwddd1", 6);
    int rc =  zmq_connect(zSocketClient, "ipc:///data/local/tmp/zmq_server.sock");
    if (rc != 0) {
        perror("zmq_connect failed");
    } else {
        printf("zmq_connect success\n");

        ipc::Request req;
        req.set_id(1);
        req.set_command("ping");

        std::string req_payload;
        req.SerializeToString(&req_payload);

        ipc::IpcRequest ipc_req;
        ipc_req.set_method("add");
        ipc_req.set_payload(req_payload);
        ipc_req.set_request_id(1001);

        std::string ipc_payload;
        ipc_req.SerializeToString(&ipc_payload);

        zmq_send(zSocketClient, "", 0, ZMQ_SNDMORE);
        zmq_send(zSocketClient, ipc_payload.data(), ipc_payload.size(), 0);
        printf("Request sent.\n");

        zmq_msg_t msg;

        zmq_msg_init(&msg);
        rc = zmq_msg_recv(&msg, zSocketClient, 0);
        if (rc >= 0) {
            printf("Received empty frame: size=%d\n", rc);
        }
        zmq_msg_close(&msg);

        zmq_msg_init(&msg);
        rc = zmq_msg_recv(&msg, zSocketClient, 0);
        if (rc >= 0) {
            std::string replyStr((char*)zmq_msg_data(&msg), zmq_msg_size(&msg));
            printf("Received reply: %s\n", replyStr.c_str());
        } else {
            printf("Failed to receive reply, errno: %d\n", errno);
        }
        zmq_msg_close(&msg);
    }
}

void generate_uuid(const char* method, char* outBuffer, size_t size) {
    pthread_once(&seed_once, init_rand_seed);

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    long long boot_ms = (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    pid_t pid = getpid();

    unsigned int tid_seed = (unsigned int)(time(NULL) ^ pthread_self());
    int rand_part = rand_r(&tid_seed) % 100000;

    snprintf(outBuffer, size, "%s_%d_%lld_%05d", method ? method : "null", pid, boot_ms, rand_part);
}

int main() {
    printf("This is client ipc\n");

    initClientSocketIpcEnv();

    connectServer();

    while (1) {
        sleep(100);
    }
    return 0;
}