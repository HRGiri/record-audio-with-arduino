import serial
import wave, struct, math, random
usb_count = 0
usb_base_name = '/dev/ttyUSB'
print("Initializing USB port.. ", end = " ")
while True:
    try:
        usb_name = usb_base_name + str(usb_count)
        
        ser = serial.Serial(usb_name, 9600)
        break
    except:
        usb_count += 1
print("Done")

while True: 
    if(ser.in_waiting >0):
        line = ser.readline()
        print(line)
        if line[0] is 70:
            fileName = line[1:-2].decode("utf-8")
            num_bytes = 0
            while True: 
                if(ser.in_waiting >0):
                    line = ser.readline()
                    if line[0] is 83:
                        num_bytes = int(line[1:-2])
                        break
            
            file = open(fileName,'wb')
            print("Receiving " + fileName + "   Size:", num_bytes)
            num = 0
            while num < num_bytes: 
                if(ser.in_waiting >0):
                    line = ser.read()
                    file.write(line)
                    num+=1
                    if num%5120 == 0:
                        print(".",end="")
            file.close()
            print("File saved successfully!")
            
            
            
            
