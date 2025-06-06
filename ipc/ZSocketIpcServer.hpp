#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
    void* zSocketIpcCxt;

    int mSocketCount;
} ZSocketIpcEnv;


typedef void (*ZmqMessageCallback)(void* data, size_t size, void (*release_fn)(void*));

typedef struct {
    void* zmqSocket;
    ZmqMessageCallback cb;
} ZSocketIpcServerParam;

static ZSocketIpcEnv g_zsocket_env;

//int all server socket zmq ctx
void initZSocketIpcEnv(ZSocketIpcEnv *);

//init a zmq socket ipc server, return zmq socket pointer
void* initSocketIpcServer(ZSocketIpcEnv *, int, const char*);

//regist receive call back form server
void regsitReceiveCallBackSocketServer(ZSocketIpcServerParam *);