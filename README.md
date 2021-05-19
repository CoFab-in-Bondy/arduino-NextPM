# arduino-NextPM
arduino-NextPM is a simple library that permits to control Next-PM sensor from an Arduino (UNO or MEGA).

# Next-PM sensors documentation
Information about the Next-PM sensor is available on [the constructor website](https://tera-sensor.com "the constructor website")


# Importante feature of the library


| Function  | Description  |
| ------------ | ------------ |
| read_10sec(PM_DATA & data)  | Measure average consentration ot the last 10 secondes  |
| read_1min(PM_DATA & data) |  Measure average consentration ot the last minute  |
| read_5min(PM_DATA & data)  |  Measure average consentration ot the last 5 minutes  |
| void configure()  | Initialise the sensor  |
| powerOn()  | Startup the sensor  |
| shutdown()  | Shutdown the sensor  |
| cleanSensor()  | Clean fan of the sensor  |


# How to use arduino-NextPM with a simple example
    /*
     * simple_example.ino
     *
     *  (c) Copyright © 2019, 2020, 2021, IRD & Université de Bourgogne
     *  Created on: Sep 6, 2020
     *      Author: Nicolas Marilleau
     */
    
    
    //include the library
    #include "NextPM.h"
    float p1 = 0, p10 = 0,  p25 = 0;
    
    //choose the serial1 port of the arduino mega
    #define SerialPMS Serial1
    
    //load Next-PM sensor
    NextPM pm_sensor(SerialPMS);
    
    //store data at each measure
    PM_DATA data;
    
    void setup() {
      //open stream to print data on console
      Serial.begin(9600);
      
      //Configure pm sensor
      pm_sensor.configure();
    }
    
    void loop() {
    
      //read data with 1 minute measure
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

# Licensing
 GNU GENERAL PUBLIC LICENSE
 (c) Copyright © 2019, 2020, 2021, IRD & Université de Bourgogne
 
  For mor details, see [page](https://github.com/CoFab-in-Bondy/arduino-NextPM/blob/main/LICENSE "page")
