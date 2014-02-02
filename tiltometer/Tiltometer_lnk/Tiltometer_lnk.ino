#include <Wire.h>
#include <ADXL345.h>

const float alpha = 0.5;
const int ALTIMETER_PIN = 0;
const int SECONDARY_MOTOR_LED = 7;
const float XTHRESHOLD = 0.1f;
const float YTHRESHOLD = 0.1f;
const float ZTHRESHOLD = 0.1f;
 
double fXg = 0;
double fYg = 0;
double fZg = 0;
 
ADXL345 acc;
 
void setup()
{
    acc.begin();
    Serial.begin(9600);
    pinMode(SECONDARY_MOTOR_LED, OUTPUT);
    delay(100);
}
 
void loop()
{
//  int altitudeReading = analogRead(ALTIMETER_PIN);
  
//  Serial.println(altitudeReading);
  
//  if (altitudeReading >= 660 && altitudeReading <= 680) {
//    Serial.println("Altitude detected!");
    
    double pitch, roll, Xg, Yg, Zg;
    acc.read(&Xg, &Yg, &Zg);
    
    Serial.print("[");
    Serial.print(Xg);
    Serial.print(",");
    Serial.print(Yg);
    Serial.print(",");
    Serial.print(Zg);
    Serial.print("]");
    Serial.println();
    
    //Low Pass Filter
//    fXg = Xg * alpha + (fXg * (1.0 - alpha));
//    fYg = Yg * alpha + (fYg * (1.0 - alpha));
//    fZg = Zg * alpha + (fZg * (1.0 - alpha));
 
    //Roll & Pitch Equations
//    roll  = (atan2(-fYg, fZg)*180.0)/M_PI;
//    pitch = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;
 
//    Serial.print(pitch);
//    Serial.print(":");
//    Serial.println(roll);

/*    if (fXg >= XTHRESHOLD ||
        fYg >= YTHRESHOLD ||
        fZg >= ZTHRESHOLD) {
      FireSecondaryMotor();
      return;
    }
*/        
    delay(1500);
//    Serial.println(analogRead(ALTIMETER_PIN));
//  }
 
}

void FireSecondaryMotor() {
  digitalWrite(SECONDARY_MOTOR_LED, HIGH);
}
