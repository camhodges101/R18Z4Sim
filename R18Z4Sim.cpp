// R18Z4Sim.cpp : Defines the entry point for the application.
//

#include "R18Z4Sim.h"

#include <iostream>
#include <cmath>
/*
This is an idealised simulator of the 4 cyl ICE engine

Each major component (Throttle, crank, pistons) are created as a class

Each component class has a constructor that sets its initial condition

Each component also has a update method, this updates the components state

All states are driven by the crankshaft position, the crankshaft is updated by 1 deg each timestep

The engine is also created as a class, it has a method run which updates all subcomponents using their update method.
*/

float pi = 3.1415926535;
               
float densityModel(float* temp, float* pressure) {
    /* Ideal Gas law to determine air density as function of pressure and temperature
    --Inputs Float pointers to temp and pressure values

    -- Actions None

    --  Outputs Air Density (kg/m3) Float
    */
    return 0.02897 * ((*pressure * 1.0) / (8.31446261815324 * (273.0 + *temp)));
}

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



int cycleState(float theta) {
    /*
    Function to convert crank angle (theta) to 4-stroke cycle stage
    0 = Intake
    1 = Compression
    2 = Ignition
    3 = Exhaust
    */

    int cycleid = 0;

    if (theta < 0) {
        theta = 4 * pi + theta;
    }

    if (theta >= 0 && theta < pi) {
        cycleid = 0;
    }

    else if (theta >= pi && theta < 2 * pi) {
        cycleid = 1;
    }

    else if (theta >= 2 * pi && theta < 3 * pi) {
        cycleid = 2;
    }

    else if (theta >= 3 * pi && theta < 4 * pi) {
        cycleid = 3;
    }
    return cycleid;
}


class piston
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
    float R = 8.31446261815324;
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
        //I should initalise all pistons at whatever there starting location is and calculate n P V and T, this will remove the need for the first cycle conditional statement, make it all more elegant. 


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
        ottoModel();


    }
};


class crank {
    /*
    Class defintion for crank model
    */
public:
    float speed;
    float position;
    float timestep;
    void update(float* enginespeed) {
        /*
        This method updates the crank position in one time step
        At the moment the model is based on incrementing the crank position by 1 deg each time step
        Going to control simulation loop speed to match model to real time.

        --Inputs Float pointer to engine speed

        --Actions update crank position based on rotation for 1 time step

        --Outputs None
        */
        speed = *enginespeed;
        position = fmod((position + (2 * pi / 360)), (4 * pi));

    }
};

class engine {
public:
    crank* crankshaft = new crank();
    throttle* Throttle = new throttle();
    float engineSpeed = 2000 * 2 * pi / 60;
    float* throttleObj;
    int i;
    int numCyl;
private:
    float offsetAngles[4] = { 0.0f, 2.0f * pi, pi, 3.0f * pi };
    piston* cylList[4];
    int k = 0;


public:
    engine(int numcyl) {

        numCyl = numcyl;
        
        
        for (i = 0; i < numcyl; i++) {
            cylList[i] = new piston(offsetAngles[i]);
        }
    }
    void run() {
        for (k = 0; k < 720 * 8; k++) {

            crankshaft->update(&engineSpeed);
            for (i = 0; i < numCyl; i++) {
                cylList[i]->intakePressure = Throttle->MAP;
                cylList[i]->update(&(crankshaft->position));
            }
            Throttle->update(&engineSpeed);
            std::cout << (cylList[0]->position) <<","<< (cylList[0]->volume)<<"," << (cylList[0]->pressure) << std::endl;
            

            //Need a sleep step here. 
        }

    }


};
int main() {
    std::cout << "starting" << std::endl;
    engine R18Z4(4);
    R18Z4.run();

}