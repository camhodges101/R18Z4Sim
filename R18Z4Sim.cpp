// R18Z4Sim.cpp : Defines the entry point for the application.
//


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
Todo List
Improve Dynamic speed control (based on throttle position via transfer function (some lag))
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
    /*
    Class to allow for a pre determined throttle track to be imported, this is a 12min track of different throttle positions. 
    --Inputs None

    -- Actions Creates STL Vector for time and throttle position, this then gets queried by time later, the timer struct gets used to return the elapsed time since the model started in seconds, this is then used to return the correct throttle position for that time. 

    --  Outputs Throttle position and matching engine speed at a given time, these are returned as floats.
    */

	public:
		int trackLength = 0;
        float targetSpeed;
		std::ifstream myFile;
		
		std::string line;
		std::vector<float>time;
		std::vector<float>throttlePercentage;
		
		speedController(){
			myFile.open("data/throttleProfile.csv");//the dir of the source file is hard coded, this will be fixed in a future release. 
			if (myFile.is_open()){
				getline(myFile,line,'\n');
				
				while(!myFile.eof()){
					std::string t,thr_per;
					getline(myFile, t,',');//Assumes source file is comma separated
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
            //This is a very simplistic transfer fuction from throttle position to engine speed, with some more research this can be improved. 
            targetSpeed =(900.0 + (5000*throttlePercentage[time])); 
            
            return targetSpeed;
        }
};







class engine {
public:
    /*
    I keep getting memory access violations if I stack allocate these objects not sure why, so here they are on the heap. 
    */
    crank* crankshaft = new crank();
    throttle* Throttle = new throttle();
    camshaft* Camshaft = new camshaft;
    speedController* SpeedReg = new speedController;
    float* engineSpeed = new float;
    
    int i;
    int numCyl;
private:
    float offsetAngles[4] = { 0.0f, 2.0f * pi, pi, 3.0f * pi }; //This is the offsets of each piston from cyl 1
    piston* cylList[4]; // array to store pointers to piston objects
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
        delete crankshaft;
        delete Throttle;
        delete Camshaft;
        delete SpeedReg;
    }
    void run() {
        Timer o_timer;
        Timer g_timer;
        for (k = 0; k < (720 * 1000); k++) {
            o_timer.startpoint();
            crankshaft->update(engineSpeed);
            Camshaft->update(&(crankshaft->position));
            // For loop to update the position/parameters of each piston, based on the new crank position and MAP value
            for (i = 0; i < numCyl; i++) {
                cylList[i]->intakePressure = Throttle->MAP;
                cylList[i]->update(&(crankshaft->position));
            }
            Throttle->update(engineSpeed);
            
            //This updates the sims internal values for engine speed and throttle position based on a predetermined time based sequence stores in a csv. 
            timestep = (int)((g_timer.endpoint())/1000000)+1;
            *engineSpeed = ((SpeedReg->getEngineSpeed(timestep))*2*pi/60);
            *(Throttle->throttleAngle) = 20+70*(SpeedReg->getThrottlePos(timestep));
            
            
            //Calculated the required sleep after each step to match real time target engine speed then sleep thread for that time
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
        /*
    Sends TCP messages to update the GUI with sensor values. 

    --Inputs Pointer to engine model and interface connection. 

    -- Actions Runs a 1hz loop that creates a message structure with up today model parameters and then sends via TCP/IP to the laptop running the interface GUI 

    --  Outputs  
    */
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
