#include "ZSocketIpcServer.hpp"


void initZSocketIpcEnv(ZSocketIpcEnv* env) {
    memset(env, 0, sizeof(ZSocketIpcEnv));

    env->zSocketIpcCxt = zmq_ctx_new();
    env->mSocketCount = 0; 
}


void* initSocketIpcServer(ZSocketIpcEnv* env, int socketType, const char* socketPath) {

    void* socket = zmq_socket(env->zSocketIpcCxt, socketType);

    if (!socket) {
        perror("zmq_socket");
        return nullptr;
    }

    if (zmq_bind(socket, socketPath) != 0) {
        perror("zmq_bind");
        zmq_close(socket);
        return nullptr;
    }

    env->mSocketCount++;
    return socket;
}

void* serverReceiveThreadFunc(void* arg) {
    ZSocketIpcServerParam *parm = (ZSocketIpcServerParam*)arg;
    int rc = 0;
    while (true) {
        zmq_msg_t identity, msg;

        zmq_msg_init(&identity);
        rc = zmq_msg_recv(&identity, parm->zmqSocket, 0);

        zmq_msg_init(&msg);
        rc = zmq_msg_recv(&msg, parm->zmqSocket, 0);

        printf("zmq_msg_recv result is %d", rc);

        size_t size = zmq_msg_size(&msg);
        void* src = zmq_msg_data(&msg);

        void* buffer = malloc(size);
        if (!buffer) {
            fprintf(stderr, "malloc failed\n");
            zmq_msg_close(&msg);
            continue;
        }

        memcpy(buffer, src, size);

        zmq_msg_close(&msg);

        if(parm->cb) {
            (* parm->cb)(buffer, size, free);
        }
    }
    return NULL;
}

/**
 * 注册SocketServer的回调。
 */
void regsitReceiveCallBackSocketServer(ZSocketIpcServerParam* p) {
    printf("- - - - start regsit receive call back socket server - - - -\n");

    pthread_t thread;

    pthread_create(&thread, NULL, serverReceiveThreadFunc, (void*)p);
    pthread_detach(thread);

    printf("- - - - end regsit receive call back socket server - - - \n");
}


/**
 * 回调之后处理函数，这个是安全处理
 */
void on_message(void* data, size_t size, void (*release_fn)(void*)) {
    printf("Callback: received message (%zu bytes): ", size);
    fwrite(data, 1, size, stdout);
    printf("\n");

    if (release_fn) {
        release_fn(data);
    }
}


int main() {
    initZSocketIpcEnv(&g_zsocket_env);

    printf("initZSocketIpcEnv invoke complete, socket env pointer address is: %p\n", g_zsocket_env.zSocketIpcCxt);

    void* repSocket = initSocketIpcServer(&g_zsocket_env, ZMQ_ROUTER, "ipc:///data/local/tmp/zmq_server.sock");

    if (repSocket != nullptr) {
        printf("create socket server success\n");
    } else {
        printf("create socket server failure\n");
    }

    ZSocketIpcServerParam parm = {
        .zmqSocket = repSocket,
        .cb = on_message
    };

    regsitReceiveCallBackSocketServer(&parm);


    while (1) {
        sleep(100);
    }
    return 0;
}
