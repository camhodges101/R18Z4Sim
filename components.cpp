#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include "includes/components.h"
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
