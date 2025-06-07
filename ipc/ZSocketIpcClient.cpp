#include "ZSocketIpcClient.hpp"

void initClientSocketIpcEnv() {
    zSocketIpcClientCxt = zmq_ctx_new();
    zSocketClient = zmq_socket(zSocketIpcClientCxt, ZMQ_DEALER);

    printf("initClientSocketIpcEnv invoke complete\n");
}

void connectServer() {
    int rc =  zmq_connect(zSocketClient, "ipc:///data/local/tmp/zmq_server.sock");
    if (rc != 0) {
        perror("zmq_connect failed");
    } else {
        printf("zmq_connect success\n");

        ipc::Request req;
        req.set_id(1);
        req.set_command("ping");

        std::string serialized;
        req.SerializeToString(&serialized);

        rc = zmq_send(zSocketClient, serialized.data(), serialized.size(), 0);
        if (rc > 0) {
            printf("zmq_send send success\n");
        } else {
            printf("zmq_send send failure\n");
        }
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