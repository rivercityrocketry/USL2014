#include <SD.h>
 
File myFile;

int angleX=0;
int angleY=0;
int angleZ=0;

int Read =1;
const int CHIP_SELECT_PIN = 10;

void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(CHIP_SELECT_PIN, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}
 
void loop()
{
    while (angleX <= 3)
    {
        angleX += 1;
        angleY += 2;
        angleZ += 3;
        //writes data to SD Card
        writeSD(String(String(angleX) + "\t" + angleY + "\t" + angleZ));
    }

    if (Read == 1)  
    {
        //reads data from SD Card
        readSD(); 
        Read = 0;
    }
}

void writeSD(String message)
{
  // open the file. Note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("Data.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) 
  {
    Serial.print("Writing to file...");
    myFile.println(message);

    // close the file:
    myFile.close();
    Serial.println("done.");
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }
}

void readSD()
{
  myFile = SD.open("Data.txt");
  if (myFile) 
  {
    Serial.println("Reading file...");
    // read from the file until there's nothing else in it:
    while (myFile.available()) 
    {
    	Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    Serial.println("Finished reading file...");
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("error opening Data.txt");
  }
}

// Delete the provided file.
void deleteFile(char* fileName)
{
  Serial.println(String("Deleting file " + String(fileName) + "..."));
  SD.remove(fileName);
  Serial.println("File deleted.");
}
