# Record audio with Arduino
In this project, I use an Arduino UNO to record audio, that can be used for Speech Recognition purposes, with a MAX4466 Electret Microphone Amplifier.

## Components Used
#### 1. MAX4466 Electret Microphone Amplifier
This is a fully assembled and tested board, that comes with a 20-20KHz electret microphone soldered on. For the amplification, the Maxim MAX4466, an op-amp specifically designed for audio recording, is used. The amplifier has excellent power supply noise rejection, so this amplifier sounds really good.
#### 2. Arduino UNO
For audio recording, we need to have a sampling rate of 44.1 kHz. While the Arduino ADC can theoretically sample at a maximum of about 77 KhZ, I am recording at about 16 kHz and still getting a decent recording, that's good should be good enough for Speech Recognition. That being said, for higher quality recording, choosing an Arduino that has a higher resolution ADC is suggested.

## Connections
** Microphone - pin A5

** Push Button - pin 2

** LED - pin 4

SD card attached to SPI bus as follows:

** MOSI - pin 11

** MISO - pin 12

** CLK - pin 13

** CS - pin 10

## Instructions
1. Open the arduino_script.ino in Arduino IDE.
2. Configure the board and Serial Port.

For Arduino setup instructions, https://www.arduino.cc/en/Guide/ArduinoUno

3. Upload the script and start recording!
#### Note: The recording is set for a preset 100,000 samples, or about 6 seconds. Change the num_samples variable in the record function for desired record time.
## Transferring the recording to Raspberry Pi (Optional)
If you want the recording for further processing in a RPi, follow these steps.
1. Connect the Arduino to the RPi via USB.
2. Run the rpi_script.py on your RPi.
