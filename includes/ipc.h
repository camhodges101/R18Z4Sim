#ifndef ipc
#define ipc

struct interfacemsg{

    public:
        float EngSPD;
        int throttlePercentage;
        float MAP;
        float MAF;
        float IntakeAirTemp;
        float AFRatio;
        float waterTemp;
        interfacemsg(float EngSpd_input, int throttlePercentage_input, float MAP_input, float MAF_Input, float IntakeAirTemp_input, float AFRatio_input, float waterTemp_input);
};


class interfaceConnection{
    public:
        int* clientSocket = new int;
        int network_socket;
        int i;
        interfaceConnection();
        ~interfaceConnection();
        int connectIPC();
        int senddata(interfacemsg* msg);
};
#endif