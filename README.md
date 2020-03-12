# Record audio with Arduino and Raspberry Pi
In this project, I use an Arduino UNO and Raspberry Pi 3 to record audio, that can be used for Speech Recognition purposes, with a MAX4466 Electret Microphone Amplifier.

## Components Used
#### 1. MAX4466 Electret Microphone Amplifier
This is a fully assembled and tested board, that comes with a 20-20KHz electret microphone soldered on. For the amplification, the Maxim MAX4466, an op-amp specifically designed for audio recording, is used. The amplifier has excellent power supply noise rejection, so this amplifier sounds really good.
#### 2. Arduino UNO
For audio recording, we need to have a sampling rate of 44.1 kHz. While the Arduino ADC can theoretically sample at a maximum of about 77 KhZ, I am recording at about 16 kHz and still getting a decent recording, that's good should be good enough for Speech Recognition. That being said, for higher quality recording, choosing an Arduino that has a higher resolution ADC is suggested.
