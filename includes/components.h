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

class piston
/*
This class defines a single piston, this is replicated in the engine class for each piston, the constructor takes the pistons offset from TDC of piston 1. The constructor sets the intake pressure to 1ATM which is then updated to manifold pressure by the throttle class. 
*/
{
public:
    float pressure;
    const float stroke = 87.3e-3f;
    const float bore = 81e-3f;
    const float rodlength = 157.5e-3f;
    const float headvolume = 0.0000468f;
    float position;
    float angleOffset;
    int cyclestate;
    const float topPosition = (0.5f * stroke) + rodlength;
    const float bottomPosition = -(0.5f * stroke) + rodlength;
    float ATMpress;
    float intakePressure;
    float volume;
    float ignition;
    float heatLoss;
    float Temp;
    int firstCycle = 0;
    const float R = 8.31446261815324;
    float n;

    piston(float TDC_Angle)

    void ottoModel()

    void update(float* crankAngle)
};


#endif