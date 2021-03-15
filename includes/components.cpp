#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include "components.h"

float densityModel(float* temp, float* pressure) {
    /* Ideal Gas law to determine air density as function of pressure and temperature
    --Inputs Float pointers to temp and pressure values

    -- Actions None

    --  Outputs Air Density (kg/m3) Float
    */
    return 0.02897 * ((*pressure * 1.0) / (8.31446261815324 * (273.0 + *temp)));
}

const float pi = 3.1415926535;

crank::crank(){
    speed =0;
    position=0;
    timestep=0;
}
void crank::update(float* enginespeed) { 
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



camshaft::camshaft(){
    position =0; 
}
void camshaft::update(float* crankAngle){
        /*
    This method updates the cam position in one time step

    --Inputs Float pointer to crank position

    --Actions update cam position based on rotation for 1 time step

    --Outputs None
    */
    position = (*crankAngle)*0.5;
}



/*
Class definition for throttle and intake model
*/


throttle::throttle() {
    VE = 0.85; // assuming 85% volumetric efficiency, this will get updated to be dynamic in a future release. 
    *throttleAngle = 20; // initialise to 20 deg throttle angle. 
    dia = 60e-3; // 60mm throttle flow diameter. 
    velocity = 0;
    flowarea = (pi * pow((dia * 0.5), 2)) - (dia * 0.5 * (0.5 * dia * cos(*throttleAngle * 2 * pi / 360) * pi));
    MAP = 101e3; //initalise to atmosphereic pressure
    MAF = 0;
    TotalAirDemand = 0;


}

void throttle::update(float* EngineSpeed) {
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
    flowarea = (pi * pow((dia * 0.5), 2)) - (dia * 0.5 * (0.5 * dia * cos(*throttleAngle * 2 * pi / 360) * pi));
    velocity = TotalAirDemand / flowarea;
    MAP = std::max((ATMPress + 0.5 * ATMrho * (0 - pow(velocity, 2))), 0.0);
    MAF = ATMrho * flowarea * velocity;
}






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