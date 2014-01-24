#include <Wire.h>
#include <ADXL345.h>

const float alpha = 0.5;
 
double fXg = 0;
double fYg = 0;
double fZg = 0;
 
ADXL345 acc;
 
void setup()
{
    acc.begin();
    Serial.begin(9600);
    delay(100);
}
 
void loop()
{
    double pitch, roll, Xg, Yg, Zg;
    acc.read(&Xg, &Yg, &Zg);
 
    //Low Pass Filter
    fXg = Xg * alpha + (fXg * (1.0 - alpha));
    fYg = Yg * alpha + (fYg * (1.0 - alpha));
    fZg = Zg * alpha + (fZg * (1.0 - alpha));
 
    //Roll & Pitch Equations
    roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
    pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;
 
    Serial.print(pitch);
    Serial.print(":");
    Serial.println(roll);
 
    delay(10);
}

/*
// The SPI library is used for...SPI communication.
#include <SPI.h>

// Accept chip siganls on pin 10.
int CS=10;

// Some of the AXDL345 registers we have access to:
char POWER_CTL = 0x2D;
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;   //X-Axis Data 0
char DATAX1 = 0x33;   //X-Axis Data 1
char DATAY0 = 0x34;   //Y-Axis Data 0
char DATAY1 = 0x35;   //Y-Axis Data 1
char DATAZ0 = 0x36;   //Z-Axis Data 0
char DATAZ1 = 0x37;   //Z-Axis Data 1

// An array of readings from the accelerometer.
char dataReadings[10];

// Current x/y/z values.
int x, y, z;

void setup() {
    SPI.begin();

    // Required for communication with AXDL.    
    SPI.setDataMode(SPI_MODE3);
    
    Serial.begin(9600);
    
  //Set up the Chip Select pin to be an output from the Arduino.
  pinMode(CS, OUTPUT);
  
  //Before communication starts, the Chip Select pin needs to be set high.
  digitalWrite(CS, HIGH);
  
  //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
  writeRegister(DATA_FORMAT, 0x01);
  //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeRegister(POWER_CTL, 0x08);  //Measurement mode  
}

void loop() {
  //Reading 6 bytes of data starting at register DATAX0 will retrieve the x,y and z acceleration values from the ADXL345.
  //The results of the read operation will get stored to the values[] buffer.
  readRegister(DATAX0, 6, dataReadings);

  //The ADXL345 gives 10-bit acceleration values, but they are stored as bytes (8-bits). To get the full value, two bytes must be combined for each axis.
  //The X value is stored in values[0] and values[1].
  x = ((int)dataReadings[1]<<8)|(int)dataReadings[0];
  //The Y value is stored in values[2] and values[3].
  y = ((int)dataReadings[3]<<8)|(int)dataReadings[2];
  //The Z value is stored in values[4] and values[5].
  z = ((int)dataReadings[5]<<8)|(int)dataReadings[4];
  
  //Print the results to the terminal.
  Serial.print(x, DEC);
  Serial.print(',');
  Serial.print(y, DEC);
  Serial.print(',');
  Serial.println(z, DEC);      
  delay(100);
}

//This function will write a value to a register on the ADXL345.
//Parameters:
//  char registerAddress - The register to write a value to
//  char value - The value to be written to the specified register.
void writeRegister(char registerAddress, char value){
  //Set Chip Select pin low to signal the beginning of an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the register address over SPI.
  SPI.transfer(registerAddress);
  //Transfer the desired register value over SPI.
  SPI.transfer(value);
  //Set the Chip Select pin high to signal the end of an SPI packzet.
  digitalWrite(CS, HIGH);
}

//This function will read a certain number of registers starting from a specified address and store their values in a buffer.
//Parameters:
//  char registerAddress - The register addresse to start the read sequence from.
//  int numBytes - The number of registers that should be read.
//  char * values - A pointer to a buffer where the results of the operation should be stored.
void readRegister(char registerAddress, int numBytes, char * values){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  if(numBytes > 1)address = address | 0x40;
  
  //Set the Chip select pin low to start an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the starting register address that needs to be read.
  SPI.transfer(address);
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  for (int i = 0; i <
  while (SPI.transfer(0x00))
//  for(int  SPI.transfer(0x00));
//  }
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(CS, HIGH);
}
/*void readRegister(char registerAddress, int numBytes, char * values){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  if(numBytes > 1)address = address | 0x40;
  
  //Set the Chip select pin low to start an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the starting register address that needs to be read.
  SPI.transfer(address);
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  while (SPI.transfer(0x00)) {} //  for(int SPI.transfer(0x00));
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(CS, HIGH);
}*/



//void setup() {
//    Serial.begin(9600);
//    Serial.print("Setup");
//}

//void loop() {
//  Serial.print("Testing");
//  delay(1000);
/*  int altitudePassed;
  
  altitudePassed = readSensor();
  
  if (altitudePassed == 1) {
    int tiltThresholdPassed;
    
    tiltThresholdPassed = isThresholdPassed();
    
    if (tiltThresholdPassed == 0) {
      fireSecondaryEngine();
    }
    
    // Exit the program after we've either fired the secondary 
    // engine or not, so that we don't continue to try firing it.
    return;
  }*/
//}
