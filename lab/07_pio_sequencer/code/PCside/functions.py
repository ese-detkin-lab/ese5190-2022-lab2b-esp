import os
import serial.tools.list_ports
def selectCOM():
    l_com=[]
    l_comde=[]
    comlist= list(serial.tools.list_ports.comports())
    for i in range(0,len(comlist)):
        l_com.append(comlist[i].device)
        l_comde.append( comlist[i].description)
        print("[{}],\t{},\t{}".format(i, comlist[i].device, comlist[i].description))
    return l_com

def mkdir():

    file_path = os.getcwd() 
    if os.path.exists(file_path + "\\records"):
        print("already there")
    else:
        os.makedirs(file_path + "\\records")
mkdir()
