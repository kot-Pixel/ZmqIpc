#include "ZSocketIpcWorker.hpp"


void* socketWorkerReceiveThread(void* arg) {
    IpcBinderWorker *worker = (IpcBinderWorker*)arg;
    char threadName[16];
    snprintf(threadName, sizeof(threadName), "ipc_worker_%d", worker->index);
    pthread_setname_np(pthread_self(), threadName);

    int rc = 0;
    worker->proxySocket = zmq_socket(worker->zSocketIpcCxt, ZMQ_DEALER);
    zmq_connect(worker->proxySocket, Z_SOCKET_SERVER_PROXY);
    
    printf("socketWorkerReceiveThread create and start\n");

    while (true) {

        zmq_msg_t identity, empty, content;

        zmq_msg_init(&identity);
        rc = zmq_msg_recv(&identity, worker->proxySocket, 0);
        printf("threadNamd %s, Worker received: identity %.*s\n", threadName,(int)zmq_msg_size(&identity), (char*)zmq_msg_data(&identity));
        
        zmq_msg_init(&empty);
        rc = zmq_msg_recv(&empty, worker->proxySocket, 0);

        zmq_msg_init(&content);
        rc = zmq_msg_recv(&content, worker->proxySocket, 0);

        printf("Worker received: content %.*s\n", (int)zmq_msg_size(&content), (char*)zmq_msg_data(&content));


        zmq_msg_send(&identity, worker->proxySocket, ZMQ_SNDMORE);
        zmq_msg_send(&empty, worker->proxySocket, ZMQ_SNDMORE);

        const char* reply = "World";
        zmq_msg_t reply_msg;
        zmq_msg_init_size(&reply_msg, strlen(reply));
        memcpy(zmq_msg_data(&reply_msg), reply, strlen(reply));
        zmq_msg_send(&reply_msg, worker->proxySocket, 0);

        // size_t size = zmq_msg_size(&msg);
        // void* src = zmq_msg_data(&msg);

        // void* buffer = malloc(size);
        // if (!buffer) {
        //     fprintf(stderr, "malloc failed\n");
        //     zmq_msg_close(&msg);
        //     continue;
        // }

        // memcpy(buffer, src, size);

        // zmq_msg_close(&msg);

        // if(parm->cb) {
        //     (* parm->cb)(buffer, size, free);
        // }
        zmq_msg_close(&reply_msg);
        zmq_msg_close(&identity);
        zmq_msg_close(&empty);
        zmq_msg_close(&content);
    }
    return NULL;
}