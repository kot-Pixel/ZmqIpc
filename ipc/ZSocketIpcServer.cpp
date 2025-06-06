#include "ZSocketIpcServer.hpp"


void initZSocketIpcEnv(ZSocketIpcEnv* env) {
    memset(env, 0, sizeof(ZSocketIpcEnv));

    env->zSocketIpcCxt = zmq_ctx_new();
    env->mSocketCount = 0; 
}


// void* registSocketIpcServer(char* socketPath,int mode) {
//     void* context = zmq_socket_new();
// }


int main() {
    //initZSocketIpcEnv(&g_zsocket_env);
    return 0;
}
