#ifndef ipc
#define ipc


class interfaceConnection{
    public:
        int clientSocket;
        int network_socket;
        int i;
        interfaceConnection();
        ~interfaceConnection();
        int connectIPC();
        int senddata(char* msg);
};
#endif