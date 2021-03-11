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

#endif