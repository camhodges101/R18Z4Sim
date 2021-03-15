import tkinter as tk
import socket
import struct
from time import sleep
from math import pi

class interface():
    def connect(self):
        
        print("Running Connect")
        UDP_IP = "192.168.0.108"
        UDP_PORT = 54000

        sock = socket.socket(socket.AF_INET, # Internet
                                socket.SOCK_STREAM) # UDP
        sock.connect((UDP_IP, UDP_PORT))
        while True:
            
            data, addr = sock.recvfrom(4096)

            data = struct.unpack("fifffff",data)
            engspd, ThrottlePosition, MAP, MAF, AirTemp, AFRatio, WaterTemp = data
            print(engspd)
            self.parameter1Data.delete(0,"end")
            self.parameter1Data.insert(0, round(engspd*60/2/pi,0))
            
            self.parameter2Data.delete(0,"end")
            self.parameter2Data.insert(0, ThrottlePosition)
            
            self.parameter3Data.delete(0,"end")
            self.parameter3Data.insert(0, round(MAP,2))
            
            self.parameter4Data.delete(0,"end")
            self.parameter4Data.insert(0, round(MAF*100,2))
            
            self.parameter5Data.delete(0,"end")
            self.parameter5Data.insert(0, AirTemp)
            
            self.parameter6Data.delete(0,"end")
            self.parameter6Data.insert(0, AFRatio)
            
            self.parameter7Data.delete(0,"end")
            self.parameter7Data.insert(0, WaterTemp)
            self.form.update()

            sleep(0.5)
    def __init__(self):
        self.form=tk.Tk()
        self.form.geometry("650x500")
        self.form.wm_title("Engine Sim Interface")
        
        self.parameters=tk.LabelFrame(self.form,text="Simulated Engine Parameters")
        self.parameters.grid(row=0,columnspan=7,sticky="W",padx=5,pady=5)
        self.header= tk.Label(self.parameters,text="Value")
        self.header.grid(row=1,column=2)
        self.header1= tk.Label(self.parameters,text="Voltage")
        self.header1.grid(row=1,column=3)
        self.parameter1Label=tk.Label(self.parameters,text="Engine Speed   ")
        self.parameter1Label.grid(row=2,column=0)
        self.parameter1Data = tk.Entry(self.parameters,width =10)
        self.parameter1Data.grid(row=2,column=2)


        self.parameter2Label=tk.Label(self.parameters,text="Throttle Position   ")
        self.parameter2Label.grid(row=3,column=0)
        self.parameter2Data = tk.Entry(self.parameters,width =10)
        self.parameter2Data.grid(row=3,column=2)
        self.parameter2voltage = tk.Entry(self.parameters,width =10)
        self.parameter2voltage.grid(row=3,column=3)
        

        self.parameter3Label=tk.Label(self.parameters,text="MAP   ")
        self.parameter3Label.grid(row=4,column=0)
        self.parameter3Data = tk.Entry(self.parameters,width =10)
        self.parameter3Data.grid(row=4,column=2)
        self.parameter3voltage = tk.Entry(self.parameters,width =10)
        self.parameter3voltage.grid(row=4,column=3)

        self.parameter4Label=tk.Label(self.parameters,text="MAF   ")
        self.parameter4Label.grid(row=5,column=0)
        self.parameter4Data = tk.Entry(self.parameters,width =10)
        self.parameter4Data.grid(row=5,column=2)
        self.parameter4voltage = tk.Entry(self.parameters,width =10)
        self.parameter4voltage.grid(row=5,column=3)

        self.parameter5Label=tk.Label(self.parameters,text="Intake Air Temp   ")
        self.parameter5Label.grid(row=6,column=0)
        self.parameter5Data = tk.Entry(self.parameters,width =10)
        self.parameter5Data.grid(row=6,column=2)
        self.parameter5voltage = tk.Entry(self.parameters,width =10)
        self.parameter5voltage.grid(row=6,column=3)


        self.parameter6Label=tk.Label(self.parameters,text="A/F Ratio   ")
        self.parameter6Label.grid(row=7,column=0)
        self.parameter6Data = tk.Entry(self.parameters,width =10)
        self.parameter6Data.grid(row=7,column=2)
        self.parameter6voltage = tk.Entry(self.parameters,width =10)
        self.parameter6voltage.grid(row=7,column=3)


        self.parameter7Label=tk.Label(self.parameters,text="Water Temp   ")
        self.parameter7Label.grid(row=8,column=0)
        self.parameter7Data = tk.Entry(self.parameters,width =10)
        self.parameter7Data.grid(row=8,column=2)
        self.parameter7voltage = tk.Entry(self.parameters,width =10)
        self.parameter7voltage.grid(row=8,column=3)


        self.controls=tk.LabelFrame(self.form,text="")
        self.controls.grid(row=1,columnspan=7,sticky="W",padx=5,pady=5)
        self.runBtn = tk.Button(self.controls, text="Connect...",command =self.connect)
        self.runBtn.grid()

        
        self.form.mainloop()


simInterface = interface()