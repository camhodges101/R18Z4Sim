# Hardware in Loop R18Z4 Engine Model

The aim of this project is to create a engine simulation in C++ that allows for sensor values to be calculated and sent via a DAC/PWM output to a engine ECM to simulate a running engine. 

If successful it will provide accurate enough sensor values that firstly, a factory ECM will run without throughing any diagnositcs and secondly, it could serve as a test bench to develop an engine ECM design from scratch. In the case of the second option this would include a custom designed PCB and control software. 

The overall system model is as follows;

![](images/modelconcept)

 
 
