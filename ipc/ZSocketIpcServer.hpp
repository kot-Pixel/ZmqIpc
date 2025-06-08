#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "ZSocketIpcWorker.hpp"
#include "ZSocketIpcUtils.hpp"
#include "ZSocketIpcDefine.hpp"
#define WORKER_COUNT 8

typedef struct {
    void* zSocketIpcCxt;
    void* mServerSocket;
    void* mServerProxySocket;

    IpcBinderWorker mServerProxyWorks[WORKER_COUNT];
} ZSocketIpcEnv;


// typedef void (*ZmqMessageCallback)(void* data, size_t size, void (*release_fn)(void*));

// typedef struct {
//     void* zmqSocket;
//     ZmqMessageCallback cb;
// } ZSocketIpcServerParam;

static ZSocketIpcEnv g_zsocket_env;

//int all server socket zmq ctx
// void initZSocketIpcEnv(ZSocketIpcEnv *);

//init a zmq socket ipc server, return zmq socket pointer
void initSocketIpcServer(ZSocketIpcEnv *);

//regist receive call back form server
// void regsitReceiveCallBackSocketServer(ZSocketIpcServerParam *);