//include the library
#include "NextPM.h"
#include "SoftwareSerial.h"

SoftwareSerial serialPort (8, 9); // RX, TX
float p1 = 0, p10 = 0,  p25 = 0;

//choose the software serial port of the arduino UNO
#define SerialPMS serialPort

// //load Next-PM sensor
NextPM pm_sensor(SerialPMS);

//store data at each measure
PM_DATA data; 

void setup() {
  //open stream to print data on console
  
  //Configure pm sensor
  Serial.begin(9600);
  pm_sensor.configure();
}

void loop() {
  // read data with 1 minute measure
  if (pm_sensor.read_1min(data))
  {
    p1 = data.PM_UG_1_0;
    p10 = data.PM_UG_10_0;
    p25 = data.PM_UG_2_5;
    Serial.println(" -- OK!");
    Serial.print("Data:");

    Serial.print("PM 1.0 (ug/m3): ");
    Serial.print(p1);

    Serial.print(" -- PM 2.5 (ug/m3): ");
    Serial.print(p25);

    Serial.print(" -- PM 10.0 (ug/m3): ");
    Serial.print(p10);
    Serial.println(" -- OK!");
  }
  else
  {
    Serial.println("-- Failed!");
  }
}