// R18Z4Sim.cpp : Defines the entry point for the application.
//

//#include "R18Z4Sim.h"
#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <fstream>
#include <string>
#include "includes/components.h"
#include "includes/ipc.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono;
/*
Stuff to add
Speed control, sleep after crank update
External throttle control
Improeve Dynamic speed control (based on throttle position via transfer function (some lag))
TCP connection to GUI
hardware interface functions
sensor voltage transfer functions. 
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
    /*
    Simple timer struct for timing the simulation. startpoint and endpoint methods for starting and stopping the timer. allows for point to point timing or ongoing incrementing timer. 
    --Inputs None

    -- Actions Starts and runs timer. 

    --  Outputs float time elapsed since start point in microseconds. 
    */
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
            float us = duration.count() * 1000000.0f;
            return us;
        }
};

const float pi = 3.1415926535;
               



class speedController{
	public:
		int trackLength = 0;
        float targetSpeed;
		std::ifstream myFile;
		
		std::string line;
		std::vector<float>time;
		std::vector<float>throttlePercentage;
		
		speedController(){
			myFile.open("data/throttleProfile.csv");
			if (myFile.is_open()){
				getline(myFile,line,'\n');
				
				while(!myFile.eof()){
					std::string t,thr_per;
					getline(myFile, t,',');
					getline(myFile, thr_per,'\n');
					if (t.length()){
						
						time.push_back(stof(t));
						throttlePercentage.push_back(stof(thr_per)); 
						trackLength++;
                 
					}
				}
			}

		}

        float getThrottlePos(int time){
            return throttlePercentage[time];
        }

        float getEngineSpeed(int time){
            
            targetSpeed =(900.0 + (5000*throttlePercentage[time])); 
            
            return targetSpeed;
        }
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
    const float bottomPosition = (0.5f * stroke) - rodlength;
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
        position = 0.0f;
        angleOffset = TDC_Angle;
        cyclestate = 0;
        ATMpress = 101e+3f;
        ottoModel();//Update all teh cylinders to their correct thermodynamic states at the postion they are initialised. 
    }

    void ottoModel() {
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

    void update(float* crankAngle) {
        /*
        This method takes a pointer crankAngle and then updates the pistons position based on this, this also includes the offset of each piston. Includes calling the otto cycle function to update cylinder pressure, temperatures and volumes
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
        // Switch the ignition flag to signal the heat addition in the otto cycle, heatloss flag drops the temperature at the end of the power stroke.
        if ((cyclestate == 1) && (topPosition - position) < 0.005) {ignition = 1;}
        if (cyclestate == 2 && (position - bottomPosition < 0.001)) {heatLoss = 1;}
        ottoModel();


    }
};


class engine {
public:
    crank* crankshaft = new crank();
    throttle* Throttle = new throttle();
    camshaft* Camshaft = new camshaft;
    speedController* SpeedReg = new speedController;
    float* engineSpeed = new float;
    
    int i;
    int numCyl;
private:
    float offsetAngles[4] = { 0.0f, 2.0f * pi, pi, 3.0f * pi };
    piston* cylList[4];
    int k = 0;
    float updateDuration = 0;
    float requirdPeriod = 0;
    int timestep=0;

public:
    engine(int numcyl) {

        numCyl = numcyl;
        *engineSpeed = 0;
        for (i = 0; i < numcyl; i++) {
            cylList[i] = new piston(offsetAngles[i]);
        }
    }
    ~engine(){
        delete engineSpeed;
    }
    void run() {
        Timer o_timer;
        Timer g_timer;
        for (k = 0; k < (720 * 1000); k++) {
            o_timer.startpoint();
            crankshaft->update(engineSpeed);
            Camshaft->update(&(crankshaft->position));
            for (i = 0; i < numCyl; i++) {
                cylList[i]->intakePressure = Throttle->MAP;
                cylList[i]->update(&(crankshaft->position));
            }
            Throttle->update(engineSpeed);
            
            //This updates the sims internal values for engine speed and throttle position based on a predetermined time based sequence stores in a csv. 
            timestep = (int)((g_timer.endpoint())/1000000)+1;
            *engineSpeed = ((SpeedReg->getEngineSpeed(timestep))*2*pi/60);
            *(Throttle->throttleAngle) = 20+70*(SpeedReg->getThrottlePos(timestep));
            
            
            //Calculated the required sleep after each step to match real time target engine speed
            updateDuration = o_timer.endpoint();
            requirdPeriod = ((1/(360*(*engineSpeed*60/(2*pi))/60))*1000000)-updateDuration;

            sleep_for(microseconds((int)requirdPeriod));


        }

    }
    float getSpeed(){
        //std::cout<<*engineSpeed<<std::endl;
        return (*engineSpeed)*60/(2*pi);
    }


};



void displayParameters(engine* target,interfaceConnection* socketConnection){
    
    while(1){
        
        interfacemsg msg(*(target->engineSpeed),*(target->Throttle->throttleAngle),(target->Throttle->MAP),(target->Throttle->MAF),0.0,0.0,0.0);
        std::cout<<sizeof(msg)<<std::endl;
        socketConnection->senddata(&msg);
        sleep_for(seconds(1));
    }
};


int main() {
    float duration;
    interfaceConnection o_interface;
    Timer s_timer;
    engine R18Z4(4);
    o_interface.connectIPC();//This method call creates the TCP connection to the interface, this is block by design so the sim doesn't start running until the GUI is connected.
    std::thread thread_object(&engine::run, R18Z4);

    displayParameters(&R18Z4, &o_interface);
    thread_object.join();
    duration = s_timer.endpoint();
    std::cout << duration/(1000000) << std::endl;


}
