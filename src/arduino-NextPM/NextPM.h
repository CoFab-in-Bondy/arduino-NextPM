/*
 * NextPM.h
 *
 *	(c) Copyright © 2019, 2020, 2021, IRD & Université de Bourgogne
 *  Created on: Sep 6, 2020
 *      Author: Nicolas Marilleau
 */


#ifndef NEXTPM_H_
#define NEXTPM_H_
#include "PM_Sensor.h"

#define NEXTPM_BAUD 115200
#define NEXT_PM_COMMAND_DELAY 500


class NextPM : public PM_Sensor {

public:
	enum CommandResponse { TIME_OUT_ERROR, CRC_ERROR,TH_MEASURE, PM_MEASURE, START_MEASURE, STOP_MEASURE,SENSOR_STATE };
	union ArrayTo32BInteger {
	 byte array[4];
	 uint32_t integer;
	};

	union ArrayTo16BInteger {
	 byte array[2];
	 unsigned int  integer;
	};


  void configure();
  bool shutdown();
  bool powerOn();
  bool read_1min(PM_DATA & data);
  bool read_10sec(PM_DATA & data);
  bool read_5min(PM_DATA & data);
  bool isActive();
  bool isSleeping();
  bool read(PM_DATA & data);
  bool readTempHumi(float & temp, float & humidity);
  void cleanSensor();
  void readSTATE(byte * data, int ml);

  NextPM(HardwareSerial& s);
  virtual ~NextPM();

private:
	ArrayTo32BInteger converter;
	float temperature;
	float humidity;
	PM_DATA sub_data;
	bool is_sleeping ;
	bool is_degraded ;
	bool is_ready ;
	bool is_temp_ok ;
	bool is_TRX_ok ;
	bool is_fan_ok ;
	bool is_memory_ok ;
	bool is_laser_ok ;

	CommandResponse readData();
	void readPM(byte * data, int ml);
	void readTempHumidity(byte * data, int ml);
	uint16_t CRC(byte *, int);
	bool checkCRC(byte * buf, int len);
	void transcodeData(PM_DATA & data);
	void emptyBuffer();
};

#endif /* NEXTPM_H_ */
