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

        ipc::IpcRequest request;
        if (!request.ParseFromArray(zmq_msg_data(&content), zmq_msg_size(&content))) {
            fprintf(stderr, "Failed to parse IpcRequest\n");
            continue;
        }

        char resp_buf[2048];
        int resp_len = 0;

        bool ok = ipc_dispatch(
            request.method().c_str(),
            request.payload().data(), request.payload().size(),
            resp_buf, &resp_len, sizeof(resp_buf)
        );

        ipc::IpcResponse response;
        response.set_request_id(request.request_id());

        if (ok) {
            response.set_payload(resp_buf, resp_len);
        } else {
            response.set_error("dispatcher error or method not found");
        }

        std::string response_data;
        response.SerializeToString(&response_data);

        zmq_msg_t reply_msg;
        zmq_msg_init_size(&reply_msg, response_data.size());
        memcpy(zmq_msg_data(&reply_msg), response_data.data(), response_data.size());

        zmq_msg_send(&identity, worker->proxySocket, ZMQ_SNDMORE);
        zmq_msg_send(&empty, worker->proxySocket, ZMQ_SNDMORE);
        zmq_msg_send(&reply_msg, worker->proxySocket, 0);
        
        zmq_msg_close(&reply_msg);
        zmq_msg_close(&identity);
        zmq_msg_close(&empty);
        zmq_msg_close(&content);
    }
    return NULL;
}