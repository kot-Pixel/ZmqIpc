#include "zmq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "ZSocketIpcDefine.hpp"
#include <pthread.h>

#define QUEUE_SIZE 10

typedef void (*ZmqMessageCallback)(void* data, size_t size, void (*release_fn)(void*));

typedef struct {
    zmq_msg_t identity;
    zmq_msg_t msg;
} task_t;


typedef struct {
    pthread_t thread;
    void* zSocketIpcCxt;
    void* proxySocket;

    int index;
    ZmqMessageCallback msgCallBack;
} IpcBinderWorker;

void* socketWorkerReceiveThread(void* arg);


