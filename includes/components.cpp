#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include "components.h"


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



/*
This class defines a single piston, this is replicated in the engine class for each piston, the constructor takes the pistons offset from TDC of piston 1. The constructor sets the intake pressure to 1ATM which is then updated to manifold pressure by the throttle class. 
*/

piston::piston(float TDC_Angle) {
    pressure = 101e3f;
    position = 0.0f;
    angleOffset = TDC_Angle;
    cyclestate = 0;
    ATMpress = 101e+3f;
    ottoModel();//Update all teh cylinders to their correct thermodynamic states at the postion they are initialised. 
}

void piston::ottoModel() {
    /*
    This is an idealised otto thermodyanmic cycle, constant head addition at combustion with idiabatic compression.

    */
    

    //Intake stroke
    if (cyclestate == 0) {
        pressure = intakePressure;
        Temp = 300.0f;
        volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
        n = (pressure * volume) / (R * Temp);


    }
    //Compression stroke
    else if (cyclestate == 1) {
        //ignition point
        if (ignition == 1) {
            Temp = 1100.0f;
            ignition = 0;
        }
        volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
        pressure = (n * R * Temp) / (volume);
                
        }
    //Power Stroke
    else if (cyclestate == 2) {
        //healoss
        if (heatLoss == 1) {
            Temp = 300.0f;
            heatLoss = 0;
        }
        volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
        pressure = (n * R * Temp) / (volume);
    }
    //exhaust stroke
    else if (cyclestate == 3) {
        Temp = 300.0f;
        volume = pi * (pow((0.5f * bore), 2)) * (topPosition - position) + headvolume;
        n = (pressure * volume) / (R * Temp);
    }


    
}

void piston::update(float* crankAngle) {
    /*
    This method takes a pointer crankAngle and then updates the pistons position based on this, this also includes the offset of each piston. Includes calling the otto cycle function to update cylinder pressure, temperatures and volumes
    --Inputs Float pointer to current Crank Angle.

    --Actions - Calculated the position of the piston and then uses the otto model to update the assocated temperature, pressure, air mass and cylinder volume. 

    --Outputs - None
    */
    float x1, y1, x2, y2;
    x1 = 0.5f * stroke * sin(*crankAngle - angleOffset);
    y1 = 0.5f * stroke * cos(*crankAngle - angleOffset);
    x2 = 0.0f;
    y2 = sqrt(pow(rodlength, 2) - pow(x1, 2)) + y1;
    position = y2;
    cyclestate = cycleState(*crankAngle);
    // Switch the ignition flag to signal the heat addition in the otto cycle, heatloss flag drops the temperature at the end of the power stroke.
    if ((cyclestate == 1) && (topPosition - position) < 0.005) {ignition = 1;}
    if (cyclestate == 2 && (position - bottomPosition < 0.001)) {heatLoss = 1;}
    ottoModel();


}




