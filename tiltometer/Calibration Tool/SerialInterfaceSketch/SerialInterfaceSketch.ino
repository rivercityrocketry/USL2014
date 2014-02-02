/******************************************************************
Author: Marco Bertschi

Copyright: Marco Bertschi  

Licence: CodeProject Open License [CPOL] 

Version: 2.0

Controller Board: Arduino Mega2560

Written with the official Arduino IDE, AVAILABLE here:
http://www.arduino.cc/en/Main/software

*******************************************************************/
/*   LIBRARIES                                                    */
#include <WString.h>              // Official Arduino string library
#include <ADXL345.h>              // Library used to get data from our accelerometer
#include <Wire.h>

/******************************************************************/
/*   CONSTANTS                                                    */
#define LED_TURN_ON_TIMEOUT  1000       //Timeout for LED power time (defines how long the LED stays powered on) in milliseconds
#define LED_PIN 13                      //Pin number on which the LED is connected
#define SERIAL_BAUDRATE 9600            //Baud-Rate of the serial Port
#define STX "\x02"                      //ASCII-Code 02, text representation of the STX code
#define ETX "\x03"                      //ASCII-Code 03, text representation of the ETX code
#define RS  "$"                         //Used as RS code

/*   WARNING, ERROR AND STATUS CODES                              */
//STATUS
#define MSG_METHOD_SUCCESS 0                      //Code which is used when an operation terminated  successfully
//WARNINGS
#define WRG_NO_SERIAL_DATA_AVAILABLE 250          //Code indicates that no new data is AVAILABLE at the serial input buffer
//ERRORS
#define ERR_SERIAL_IN_COMMAND_NOT_TERMINATED -1   //Code is used when a serial input commands' last char is not a '#'
/******************************************************************/
/*   GLOBAL VARIABLES                                             */
ADXL345 accelerometer;

double Xg = 0;
double Yg = 0;
double Zg = 0; 
/******************************************************************/
/*   METHOD DECLARATIONS                                          */
void WriteDummyData();
/******************************************************************/
/*   METHODS    
******************************************************************/
void setup(){
  accelerometer.begin();
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(SERIAL_BAUDRATE);  //setup serial pin
}

void loop(){
  String command = "";  // Used to store the latest received command
  int serialResult = 0; // return value for reading operation method on serial in put buffer
 
  accelerometer.read(&Xg, &Yg, &Zg);
  
  WriteDummyData();

  digitalWrite(LED_PIN, HIGH);
  delay(250);
  digitalWrite(LED_PIN, LOW);
  delay(250);
}

/*****************************************************************
Description:
This method writes dummy data to the serial interface output
buffer. The dummy data is formatted as 

[STX]
<data here>[ETX]

Values written in braces ("[]") Represent an ASCII control
code. For further information about ASCII codes visit
http://www.asciitable.com

Return-Values:
 - void

Version: 1.0

Autor:   Marco Bertschi

Licence: CodeProject Open License [CPOL]

******************************************************************/
void WriteDummyData(){
  Serial.print(STX);
  Serial.print("[");
  Serial.print(Xg);
  Serial.print(",");
  Serial.print(Yg);
  Serial.print(",");
  Serial.print(Zg);
  Serial.print("]");
  Serial.print(RS);
  Serial.print(ETX);
}
