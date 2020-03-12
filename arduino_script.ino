
/*
  SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 10

** Push Button - pin 2
** LED - pin 4
*/

#include <SPI.h>
#include <SD.h>


PROGMEM const byte header [44] =
 // This contains the header of a WAV file. Without this, it is not recognized as such.
 // 44 bytes long.
{
 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56,
 0x45, 0x66, 0x6D, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00,
 0x01, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x01,
 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00, 0x00, 0x00, 0x00
};

// Position in the file to write down the overall file size minus 8.
const byte firstPos = 4;
// Position in the file to write down the overall file size minus 44.
const byte secondPos = 40;
// Position in the file to write down the sampling frequency.
const byte thirdPos = 24;
const byte fourthPos = 28;

const byte button = 2; // Pin for the button.
const byte led = 4;
// Pin for the status LED. Unfortunately, the built-in LED is already
// used by the SPI.
byte buttonState = LOW;
// Remember the button state before test the loop's condition. LOW is default or
// not pressed.
File recording;
File dir;

int sample_rate;
int bitsPerSample = 16;

// Wav File name
String Wav_File;; 

void setup() {
   // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

 Serial.println("Arduino Connected!");
 pinMode(button, INPUT);
 pinMode(led, OUTPUT);
 pinMode(10, OUTPUT);
 ADCSRA = (ADCSRA & 0xf8) | 0x04; // ADC fast mode
 digitalWrite(led, LOW);

 if (!SD.begin(10)) {
   Serial.println("SD card failure!");
   errorStatus(); // SD card failure
 }
   File newFile = SD.open("rec00000.wav", FILE_WRITE);
 if (SD.exists("REC001.wav")) {
   if (!SD.remove("REC001.wav")) {
     Serial.println("SD card may be write protected or damaged!");
     errorStatus();
     // Any previous (or incomplete) recording should be deleted
     // in order to make a new one. This error might occur due to
     // write-protected or damaged card.
   }
 }
}

void loop() {

 if (digitalRead(button) == HIGH) {
	// Button pressed for recording
    delay(2000);
    dir = SD.open("/");
    //GEt and set Files on  SD card
    while (true) {
		delay(100);
		File entry =  dir.openNextFile();
         if (! entry) {
		// no more files
			entry.close();
			Serial.println("Creating a new file!");
			break;
    }
		Serial.println("    " + entry.name());
		Wav_File = String(entry.name());
		entry.close();
    }
	// Create a string for a new file name
    if (Wav_File[7] == '9') {
		Wav_File[6] = int(Wav_File[6]) + 1;
		Wav_File[7] = '0';
    }
     else{
		Wav_File[7] = int(Wav_File[7])+1;  
    }
      
	Serial.print(Wav_File);
	Serial.println(" created!");
	//Create and Open a new file for recording
	recording = SD.open(Wav_File, FILE_WRITE);
	if (recording) {
		//Write an important header for a WAV File
        writeHeader();
		buttonState = LOW;
		record();
		//Update the header
		finalize();
		// Send the created file to Raspberry Pi (Optional: Uncomment if you wish to use it)
		//sendFile(Wav_File);
		finishStatus();
   } else {
		Serial.println("SD card might be write-protected!"); // Might be write-protected card or else
		errorStatus(); 
   }

}
}



void writeHeader() {
 // Puts the "idetification" of a typical WAV file (because .wav isn't enough)
 for (byte pos = 0; pos < sizeof(header); pos++) {
   recording.write(pgm_read_byte(&header[pos]));
 }
}

void record() {
 /*
  * This is how the recording process works. This will last until (normally)
  * the press of the button.
  *
  * DO NOT RESET OR POWER-down THE BOARD WHILE IN THIS STATE, UNTIL THE STATUS LED
  * GETS IN THE FINISHED STATE; otherwise file corruption and/or SD card damage will occur.
  *
  * Exceeding 4 GiB file size (149hr 7min 50sec of audio recording), when meeting a file system's
  * limitation, or filling up the memory; may cause unexpected behavior.
  */
 int sample = 0;
 long int start = millis();
 long int time_taken;
 // Change num_samples to change record time. Current sampling rate is ~16,000 samples per sec
 long int num_samples = 100000;
 long int count = num_samples;
 
 // Switch ON the LED to indicate Recording has started 
 digitalWrite(led, HIGH);
 Serial.println("Recording started!");
  
 while (count>=0) {
   sample = analogRead(A5);
   recording.write(sample);
   count--;
 }
 //Calculate the average sampling frequency obtained
 time_taken = millis()-start;
 sample_rate = (int)((num_samples*1000)/time_taken);
 
 Serial.println("Recording finished!");
 Serial.print("Time(s): ");
 Serial.println((float)time_taken/1000);
 Serial.print("Sampling Frequency: ");
 Serial.println(sample_rate);

 digitalWrite(led, LOW);
}

void finalize() {
 // Fills up four necessary variables in the file's header,
 // then ensures a successfully saved recording.
 byte finalValue[4];
 unsigned long fileSize = recording.size();
 unsigned long riffSize = fileSize - 8;
 unsigned long dataSize = fileSize - 44;
 int val = sample_rate*bitsPerSample/8;

 finalValue[0] = riffSize & 0xff;
 finalValue[1] = (riffSize >> 8) & 0xff;
 finalValue[2] = (riffSize >> 16) & 0xff;
 finalValue[3] = (riffSize >> 24) & 0xff;
 

 recording.seek(firstPos);
 recording.write(finalValue, 4);
 // Check if already in little-endian order

 finalValue[0] = sample_rate & 0xff;
 finalValue[1] = (sample_rate >> 8) & 0xff;
 finalValue[2] = (sample_rate >> 16) & 0xff;
 finalValue[3] = (sample_rate >> 24) & 0xff;
 

 recording.seek(thirdPos);
 recording.write(finalValue, 4);

 
 finalValue[0] = val & 0xff;
 finalValue[1] = (val >> 8) & 0xff;
 finalValue[2] = (val >> 16) & 0xff;
 finalValue[3] = (val >> 24) & 0xff;
 // Is possible to make a fuction that returns an array of bytes?

 recording.seek(fourthPos);
 recording.write(finalValue, 4);

 finalValue[0] = dataSize & 0xff;
 finalValue[1] = (dataSize >> 8) & 0xff;
 finalValue[2] = (dataSize >> 16) & 0xff;
 finalValue[3] = (dataSize >> 24) & 0xff;
 

 recording.seek(secondPos);
 recording.write(finalValue, 4);
 
 recording.close();
 
}


void errorStatus() {
 // The status LED endlessly indicates a critical error.
 // The only way out, is by resetting the MCU (reset button or power down).
 while (true) {
   digitalWrite(led, HIGH);
   delay(250);
   digitalWrite(led, LOW);
   delay(250);
    Serial.println("Error: program stopped");
 }
}


void finishStatus() {
/* The status LED indicates a successfully saved recording by blinking thrice.
 */
 
 for  (int i = 0;i<3;i++) {
   digitalWrite(led, HIGH);
   delay(1000);
   digitalWrite(led, LOW);
   delay(1000);
   Serial.println("Recording done!");
 }
 Serial.println("Push button to start recording again");
}


void sendFile(String fileName){
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Serial.println("Transferring file to Rpi...");
  File dataFile = SD.open(fileName);
  unsigned long num_bytes = dataFile.size();
  //Send the filename with the first byte "F" for filename
  Serial.println("F" + fileName);
  //Send the file size with the first byte "S" for file size
  Serial.println("S" + String(num_bytes));
	delay(2000);
	//Send the data
      if (dataFile) {
      while (dataFile.available()) {
        Serial.write(dataFile.read());
    }
    dataFile.close();
   
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
  }
}
