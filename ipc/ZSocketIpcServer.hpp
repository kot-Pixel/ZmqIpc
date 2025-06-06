#include <zmq.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    void* zSocketIpcCxt;

    int mSocketCount;
} ZSocketIpcEnv;


static ZSocketIpcEnv g_zsocket_env;

//int all server socket zmq ctx
void initZSocketIpcEnv(ZSocketIpcEnv *);


//regist a zmq socket ipc server, return zmq socket pointer
// void* registSocketIpcServer(char* socketPath,int mode);