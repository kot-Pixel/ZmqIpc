#include "ZSocketIpcServer.hpp"


void initSocketIpcServer(ZSocketIpcEnv* env) {

    int rc = 0;

    memset(env, 0, sizeof(ZSocketIpcEnv));
    env->zSocketIpcCxt = zmq_ctx_new();

    if (env->zSocketIpcCxt == nullptr) {
        perror("server zmq ctx create error");
        return;
    }

    //create server socket process client.
    env->mServerSocket = zmq_socket(env->zSocketIpcCxt, ZMQ_ROUTER);

    //create server proxy socket.
    env->mServerProxySocket = zmq_socket(env->zSocketIpcCxt, ZMQ_DEALER);

    if (!env->mServerSocket) {
        perror("server zmq socket create error");
    }

    if (zmq_bind(env->mServerSocket, Z_SOCKET_SERVER_PATH) != 0) {
        perror("server zmq bind error");
        zmq_close(env->mServerSocket);
    }

    if(!env->mServerProxySocket) {
        perror("server proxy zmq socket create error");
    }

    if (zmq_bind(env->mServerProxySocket, Z_SOCKET_SERVER_PROXY) != 0) {
        perror("server proxy zmq bind error");
        zmq_close(env->mServerProxySocket);
    }

    printf("server socket and proxy socket create complete\n");

    for (int i = 0; i < WORKER_COUNT; ++i) {
        env->mServerProxyWorks[i].zSocketIpcCxt = env->zSocketIpcCxt;
        env->mServerProxyWorks[i].msgCallBack = nullptr;
        env->mServerProxyWorks[i].index = i;
    
        if (rc == 0) {
            pthread_create(&env->mServerProxyWorks[i].thread, NULL, socketWorkerReceiveThread, &env->mServerProxyWorks[i]);
            pthread_detach(env->mServerProxyWorks[i].thread);
        }
    }

    zmq_proxy(env->mServerSocket, env->mServerProxySocket, NULL);

    printf("server terminal, will close socket\n");

    zmq_close(env->mServerSocket);
    zmq_close(env->mServerProxySocket);
    zmq_ctx_term(env->zSocketIpcCxt);
}



/**
 * 注册SocketServer的回调。
 */
// void regsitReceiveCallBackSocketServer(ZSocketIpcServerParam* p) {
//     printf("- - - - start regsit receive call back socket server - - - -\n");

//     pthread_t thread;

//     pthread_create(&thread, NULL, serverReceiveThreadFunc, (void*)p);
//     pthread_detach(thread);

//     printf("- - - - end regsit receive call back socket server - - - \n");
// }


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
    // initZSocketIpcEnv(&g_zsocket_env);

    // printf("initZSocketIpcEnv invoke complete, socket env pointer address is: %p\n", g_zsocket_env.zSocketIpcCxt);

    initSocketIpcServer(&g_zsocket_env);

    // if (repSocket != nullptr) {
    //     printf("create socket server success\n");
    // } else {
    //     printf("create socket server failure\n");
    // }

    // ZSocketIpcServerParam parm = {
    //     .zmqSocket = repSocket,
    //     .cb = on_message
    // };

    // regsitReceiveCallBackSocketServer(&parm);


    // while (1) {
    //     sleep(100);
    // }
    return 0;
}
