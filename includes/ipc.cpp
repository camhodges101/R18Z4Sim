#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include "ipc.h"
using namespace std;




interfacemsg::interfacemsg(float EngSpd_input, int throttlePercentage_input, float MAP_input, float MAF_Input, float IntakeAirTemp_input, float AFRatio_input, float waterTemp_input){
        EngSPD = EngSpd_input;
        throttlePercentage = throttlePercentage_input;
        MAP = MAP_input;
        MAF = MAF_Input;
        IntakeAirTemp = IntakeAirTemp_input;
        AFRatio = AFRatio_input;
        waterTemp = waterTemp_input;
}







interfaceConnection::interfaceConnection(){}
interfaceConnection::~interfaceConnection(){
    close(*clientSocket);
}
int interfaceConnection::connectIPC(){
    int i;
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1){
        cerr << "Can't create socket!";
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(network_socket, (sockaddr*)&hint, sizeof(hint)) == -1){

    cerr << "Can't bind to IP/Port; \n";
    return -2;

    }

    //Mark the socket for listening
    if (listen(network_socket, SOMAXCONN)== -1){
        cerr << "Can't Listen";
        return -3;
    }

    // Accept Call
    

    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    
    *clientSocket = accept(network_socket, NULL, NULL);
    
    return *clientSocket;
}
int interfaceConnection::senddata(interfacemsg* msg){
    
    int sendstate = send(*clientSocket, msg, 28, 0);
    if (sendstate == -1) {
        cerr << "Send Failed"<<endl;
        cout<<"Client Socket"<<*clientSocket<<endl;
    }
    return sendstate;

}

