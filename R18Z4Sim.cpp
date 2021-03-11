// R18Z4Sim.cpp : Defines the entry point for the application.
//

//#include "R18Z4Sim.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include "includes/components.h"
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;
/*
Stuff to add
Speed control, sleep after crank update
External throttle control
Dynamic speed control (based on throttle position via transfer function (some lag))
*/

/*
This is an idealised simulator of the 4 cyl ICE engine

Each major component (Throttle, crank, pistons) are created as a class

Each component class has a constructor that sets its initial condition

Each component also has a update method, this updates the components state

All states are driven by the crankshaft position, the crankshaft is updated by 1 deg each timestep

The engine is also created as a class, it has a method run which updates all subcomponents using their update method.
*/

struct Timer{
    public:
            std::chrono::time_point<std::chrono::system_clock> start, end;
            std::chrono::duration<float>duration;
        Timer(){
            start = std::chrono::high_resolution_clock::now();
            }
        void startpoint(){
            start = std::chrono::high_resolution_clock::now();
        }
        float endpoint(){
            end = std::chrono::high_resolution_clock::now();
            duration = end - start;
            float ns = duration.count() * 1000000.0f;
            return ns;
        }
};

const float pi = 3.1415926535;
               


class speedController {
    float speedError = 0;
    float engineSpeed = 0;
};


class throttle {
    /*
    Class definition for throttle and intake model
    */
public:
    int throttleAngle;
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

    throttle() {
        VE = 0.85;
        throttleAngle = 40;
        dia = 60e-3;
        velocity = 0;
        flowarea = (pi * pow((dia * 0.5), 2)) - (dia * 0.5 * (0.5 * dia * cos(throttleAngle * 2 * pi / 360) * pi));
        MAP = 101e3;
        MAF = 0;
        TotalAirDemand = 0;


    }

    void update(float* EngineSpeed) {
        /*
        This method takes the current engine speed and calculates a total air demand for the 4 cylinders
        Need to update this if the code is to generalise for engines other than 4 cyl models
        Cylinder volume is hard coded, need to add head volume to this
        MAP is calulated with a belnoili model of flow through a reduced cross section, assumed air density and pressure at 15deg C and sea level
        MAF is assumed to be based on float at the narrowest part of the throttle body.
        Assumes 60mm throttle body

        --Inputs Float Pointer to Engine speed

        --Actions Updates public MAP and MAF values as floats.

        --Outputs None
        */
        TotalAirDemand = 4 * 0.00045 * VE * (*EngineSpeed / (2 * pi)) * 0.5;
        velocity = TotalAirDemand / flowarea;
        MAP = std::max((ATMPress + 0.5 * ATMrho * (0 - pow(velocity, 2))), 0.0);
        MAF = ATMrho * flowarea * velocity;
    }


};






class piston
/*
This class defines a single piston, this is replicated in the engine class for each piston, the constructor takes the pistons offset from TDC of piston 1. The constructor sets the intake pressure to 1ATM which is then updated to manifold pressure by the throttle class. 
*/
{
public:
    float pressure;
    float stroke;
    float bore;
    float rodlength;
    float headvolume;
    float position;
    float angleOffset;
    int cyclestate;
    float topPosition;
    float bottomPosition;
    float ATMpress;
    float intakePressure;
    float volume;
    float ignition;
    float heatLoss;
    float Temp;
    int firstCycle = 0;
    const float R = 8.31446261815324;
    float n;

    piston(float TDC_Angle) {
        pressure = 101e3f;
        stroke = 87.3e-3f;
        bore = 81e-3f;
        rodlength = 157.5e-3f;
        headvolume = 0.0000468f;
        position = 0.0f;
        angleOffset = TDC_Angle;
        cyclestate = 0;
        topPosition = (0.5f * stroke) + rodlength;
        bottomPosition = (0.5f * stroke) - rodlength;
        ATMpress = 101e+3f;
        ottoModel();
    }

    void ottoModel() {
        /*
        This is an idealised otto thermodyanmic cycle, constant head addition at combustion with idiabatic compression.

        */
        


        if (cyclestate == 0) {
            pressure = intakePressure;
            Temp = 300.0f;
            volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
            n = (pressure * volume) / (R * Temp);


        }
        else if (cyclestate == 1) {
            if (ignition == 1) {
                Temp = 1100.0f;
                ignition = 0;
            }
            volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
            pressure = (n * R * Temp) / (volume);
                    
            }
        
        else if (cyclestate == 2) {
            if (heatLoss == 1) {
                Temp = 300.0f;
                heatLoss = 0;
            }
            volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
            pressure = (n * R * Temp) / (volume);
        }
        else if (cyclestate == 3) {
            Temp = 300.0f;
            volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
            n = (pressure * volume) / (R * Temp);
        }


        
    }

    void update(float* crankAngle) {
        /*
        This method takes a pointer crankAngle and then updates the pistons position based on this, this also includes the offset of each piston.
        --Inputs

        --Actions

        --Outputs
        */
        float x1, y1, x2, y2;
        x1 = 0.5f * stroke * sin(*crankAngle - angleOffset);
        y1 = 0.5f * stroke * cos(*crankAngle - angleOffset);
        x2 = 0.0f;
        y2 = sqrt(pow(rodlength, 2) - pow(x1, 2)) + y1;
        position = y2;
        cyclestate = cycleState(*crankAngle);
        if ((cyclestate == 1) && (topPosition - position) < 0.005) {ignition = 1;}
        if (cyclestate == 2 && (position - bottomPosition < 0.001)) {heatLoss = 1;}
        ottoModel();


    }
};




class engine {
public:
    crank* crankshaft = new crank();
    throttle* Throttle = new throttle();
    float engineSpeed = 6000 * 2 * pi / 60;
    
    int i;
    int numCyl;
private:
    float offsetAngles[4] = { 0.0f, 2.0f * pi, pi, 3.0f * pi };
    piston* cylList[4];
    int k = 0;
    float updateDuration = 0;
    float requirdPeriod = 0;

public:
    engine(int numcyl) {

        numCyl = numcyl;
                
        for (i = 0; i < numcyl; i++) {
            cylList[i] = new piston(offsetAngles[i]);
        }
    }
    void run() {
        Timer o_timer;
        for (k = 0; k < (720 * 1000); k++) {
            o_timer.startpoint();
            crankshaft->update(&engineSpeed);
            for (i = 0; i < numCyl; i++) {
                cylList[i]->intakePressure = Throttle->MAP;
                cylList[i]->update(&(crankshaft->position));
            }
            Throttle->update(&engineSpeed);
            //Calculated the required sleep after each step to match real time target engine speed
            updateDuration = o_timer.endpoint();
            requirdPeriod = ((1/(6*(engineSpeed*60/(2*pi))/60))*1000000)-updateDuration;
            
            sleep_for(nanoseconds((int64_t)requirdPeriod));
            //std::cout << (cylList[0]->position) <<","<< (cylList[0]->volume)<<"," << (cylList[0]->pressure) << std::endl;
            

            //Need a sleep step here. 
        }

    }


};
int main() {
    float duration;
    engine R18Z4(4);
    Timer s_timer;
    R18Z4.run();
    duration = s_timer.endpoint();
    std::cout << duration/(1000000) << std::endl;

}
