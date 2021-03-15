#ifndef components
#define components


int cycleState(float);

float densityModel(float*, float*);


class crank {
    /*
    Class defintion for crank model
    */
public:
    float speed;
    float position;
    float timestep;
    crank();
    void update(float* enginespeed);

};



class camshaft{
    public:
        float position;
        camshaft();
        void update(float* crankAngle);
};



class throttle {
    /*
    Class definition for throttle and intake model
    */
public:
    int* throttleAngle = new int;
    float MAP;
    float MAF;
    float velocity;

private:
    float dia;
    float flowarea;
    float TotalAirDemand;
    float VE;
    float ATMPress = 101e3;
    float ATMrho = 1.225;

public:

    throttle(); 

    void update(float* EngineSpeed);

};



#endif