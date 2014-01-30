/******************************************************************
Author: Marco Bertschi

Copyright: Marco Bertschi  

Licence: CodeProject Open License [CPOL] 

Version: 2.0

Controller Board: Arduino Mega2560

Written with the official Arduino IDE, AVAILABLE here:
http://www.arduino.cc/en/Main/software

*******************************************************************/
/*   INCLUDES                                                     */
#include <WString.h> //Official Arduino string library
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
/*   METHOD DECLARATIONS                                          */
int readSerialInputString(String *command);
void WriteDummyWeatherData();
/******************************************************************/
/*   METHODS    
******************************************************************/
void setup(){
  //setup the LED pin for output
  pinMode(LED_PIN, OUTPUT);
  //setup serial pin
  Serial.begin(SERIAL_BAUDRATE);
}

void loop(){
  String command = "";  //Used to store the latest received command
  int serialResult = 0; //return value for reading operation method on serial in put buffer
  
  serialResult = readSerialInputCommand(&command);
  if(serialResult == MSG_METHOD_SUCCESS){
    if(command == "1#"){//Request for sending weather data via Serial Interface
                        //For demonstration purposes this only writes dummy data
        WriteDummyWeatherData();
    }
  }
  
  if(serialResult == WRG_NO_SERIAL_DATA_AVAILABLE){//If there is no data AVAILABLE at the serial port, let the LED blink
     digitalWrite(LED_PIN, HIGH);
     delay(250);
     digitalWrite(LED_PIN, LOW);
     delay(250);
  }
  else{
    if(serialResult == ERR_SERIAL_IN_COMMAND_NOT_TERMINATED){//If the command format was invalid, the led is turned off for two seconds
      digitalWrite(LED_PIN, LOW);
      delay(2000);
    }
  }
}

/*****************************************************************
Description:
This method reads the serial input buffer and writes the content 
of the serial input buffer to the variable which is adressed by the
pointer *command.

Return-Values:
 - MSG_METHOD_SUCCESS -> 
   ->    Serial buffer data stored to the variable which is adressed by the
         pointer *command.
 - ERR_SERIAL_IN_COMMAND_NOT_TERMINATED
   ->    Command was not terminated by a '#' (example: "command") or contains no command text (example: "#" or "").

Version: 1.1

Autor:   Marco Bertschi

Licence: CodeProject Open License [CPOL]

******************************************************************/
int readSerialInputCommand(String *command){
  
  int operationStatus = MSG_METHOD_SUCCESS;//Default return is MSG_METHOD_SUCCESS reading data from com buffer.
  
  //check if serial data is available for reading
  if (Serial.available()) {
     char serialInByte;//temporary variable to hold the last serial input buffer character
     
     do{//Read serial input buffer data byte by byte 
       serialInByte = Serial.read();
       *command = *command + serialInByte;//Add last read serial input buffer byte to *command pointer
     }while(serialInByte != '#' && Serial.available());//until '#' comes up or no serial data is available anymore
     
     if(serialInByte != '#') {
       operationStatus = ERR_SERIAL_IN_COMMAND_NOT_TERMINATED;
     }
  }
  else{//If not serial input buffer data is AVAILABLE, operationStatus becomes WRG_NO_SERIAL_DATA_AVAILABLE (= No data in the serial input buffer AVAILABLE)
    operationStatus = WRG_NO_SERIAL_DATA_AVAILABLE;
  }
  
  return operationStatus;
}
/*****************************************************************
Description:
This method writes dummy weather data to the serial interface output
buffer. The dummy data is formatted as 

[STX]
11 06 2013 12:00=11.78$
11 06 2013 23:00=8.9$
11 07 2013 06:00=4.54$
11 07 2013 12:00=15.3$
11 07 2013 23:00=8.6$
11 21 2013 06:00=-2.33[ETX]

Values written in braces ("[]") Represent an ASCII control
code. For further information about ASCII codes visit
http://www.asciitable.com

Return-Values:
 - void

Version: 1.0

Autor:   Marco Bertschi

Licence: CodeProject Open License [CPOL]

******************************************************************/
void WriteDummyWeatherData(){
  Serial.print(STX);
  Serial.print("11 06 2013 12:00=11.78");
  Serial.print(RS);
  Serial.print("11 06 2013 23:00=8.9");
  Serial.print(RS);
  Serial.print("11 07 2013 06:00=4.54");
  Serial.print(RS);
  Serial.print("11 07 2013 12:00=15.3");
  Serial.print(RS);
  Serial.print("11 07 2013 23:00=10.3");
  Serial.print(RS);
  Serial.print("11 21 2013 06:00=-2.33");
  Serial.print(ETX);
}
