#include "ZUdsRpcServer.hpp"
// #include "zmq.h"

int ZUdsRpcServer::initZUdsRpcServer()
{
    // zUdsSocketZmqCtx = zmq_ctx_new();

    // if (zUdsSocketZmqCtx == nullptr) {
    //     perror("server zmq ctx create error");
    //     return -1;
    // }
    // //create server socket process client.
    // mUdsServerSocket = zmq_socket(zUdsSocketZmqCtx, ZMQ_ROUTER);

    // //create server proxy socket.
    // mUdsServerProxySocket = zmq_socket(zUdsSocketZmqCtx, ZMQ_DEALER);

    // if (!mUdsServerSocket) {
    //     perror("server zmq socket create error");
    //     return -1;
    // }

    // if (zmq_bind(mUdsServerSocket, Z_SOCKET_SERVER_PATH) != 0) {
    //     perror("server zmq bind error");
    //     zmq_close(mUdsServerSocket);
    //     return -1;
    // }

    // if(!mUdsServerProxySocket) {
    //     perror("server proxy zmq socket create error");
    //     return -1;
    // }

    // if (zmq_bind(mUdsServerProxySocket, Z_SOCKET_SERVER_PROXY) != 0) {
    //     perror("server proxy zmq bind error");
    //     zmq_close(mUdsServerProxySocket);
    //     return -1;
    // }

    printf("server socket and proxy socket create complete\n");

    // for (int i = 0; i < WORKER_COUNT; ++i) {
    //     env->mServerProxyWorks[i].zSocketIpcCxt = env->zSocketIpcCxt;
    //     env->mServerProxyWorks[i].msgCallBack = nullptr;
    //     env->mServerProxyWorks[i].index = i;
    
    //     if (rc == 0) {
    //         pthread_create(&env->mServerProxyWorks[i].thread, NULL, socketWorkerReceiveThread, &env->mServerProxyWorks[i]);
    //         pthread_detach(env->mServerProxyWorks[i].thread);
    //     }
    // }

    // rpcWorker = new ZUdsRpcWorker(zUdsSocketZmqCtx);
    // rpcWorker->startZUdsWork();

    // zmq_proxy(mUdsServerSocket, mUdsServerProxySocket, NULL);
    return 0;
}

int main() {
    ZUdsRpcServer sexxxxr;
    sexxxxr.initZUdsRpcServer();
    return 0;
}