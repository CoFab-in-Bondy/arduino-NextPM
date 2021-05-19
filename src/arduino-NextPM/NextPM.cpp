/*
 * NextPM.cpp
 *
 *	(c) Copyright © 2019, 2020, 2021, IRD & Université de Bourgogne
 *  Created on: Sep 6, 2020
 *      Author: Nicolas Marilleau
 */
#include "Arduino.h"
#include <NextPM.h>



void NextPM::emptyBuffer()
{
	while(hstream->available())
	{
		hstream->read();
	}
}

void NextPM::configure()
{
	Serial.write("configured");
	hstream->begin(NEXTPM_BAUD,SERIAL_8E1);
}

bool NextPM::shutdown()
{
	if(isSleeping())
			return false;
	emptyBuffer();
	byte message [] = {0x81, 0x15, 0x6A};
	hstream->write(message,sizeof(message));
	delay(NEXT_PM_COMMAND_DELAY);
	return readData()==STOP_MEASURE;
}

bool NextPM::powerOn()
{
	if(isActive())
		return false;
	emptyBuffer();
	byte message [] = {0x81, 0x15, 0x6A};
	hstream->write(message, sizeof(message));
	delay(NEXT_PM_COMMAND_DELAY);
	return readData()==START_MEASURE;
}

bool NextPM::isSleeping()
{

	emptyBuffer();
	byte message [] = {0x81,0x16,0x69};
	hstream->write(message, sizeof(message));
	delay(NEXT_PM_COMMAND_DELAY);
	if(readData()==SENSOR_STATE)
		return is_sleeping;
	return false;
}

bool NextPM::isActive()
{

	emptyBuffer();
	byte message [] = {0x81,0x16,0x69};
	hstream->write(message, sizeof(message));
	delay(NEXT_PM_COMMAND_DELAY);
	if(readData()==SENSOR_STATE)
		return !is_sleeping;
	return false;
}

void NextPM::cleanSensor()
{

}

bool NextPM::read_1min(PM_DATA & data){
	byte message1 [] = {0x81,0x12,0x6D};
	emptyBuffer();
	hstream->write(message1, sizeof(message1));
	delay(NEXT_PM_COMMAND_DELAY);
	if(PM_MEASURE == this->readData())
	{
		this->transcodeData(data);
		return true;
	}
	return false;
}
bool NextPM::read_10sec(PM_DATA & data)
{
	//0x81,0x12,0x6D
	emptyBuffer();
	byte message1 [] = {0x81,0x11,0x6E};//{0x81,0x11,0x6E};
	Serial.write("\n\n\nread 10 sec");
	hstream->write(message1, sizeof(message1));

	//this->hstream->write(message1, sizeof(message1));
	delay(NEXT_PM_COMMAND_DELAY);
	delay(5000);
	if(PM_MEASURE == readData())
	{
		this->transcodeData(data);
		return true;
	}
	return false;
}
bool NextPM::read_5min(PM_DATA & data){
	emptyBuffer();
	byte message1 [] = {0x81, 0x13, 0x6C};// {0x81,0x12,0x6D};
	hstream->write(message1, sizeof(message1));
	delay(NEXT_PM_COMMAND_DELAY);
	if(PM_MEASURE == this->readData())
	{
		this->transcodeData(data);
		return true;
	}
	return false;

}

bool NextPM::read(PM_DATA & data)
{
	Serial.println("cousou");
/*	byte message1 [] = {0x7E, 0x00, 0x03, 0x00, 0xFC, 0x7E};
	this->hstream->write(message1, sizeof(message1));
	delay(NEXT_PM_COMMAND_DELAY);
	if(PM_MEASURE == this->readData())
	{
		this->transcodeData(data);
		return true;
	}*/
	return false;
}


void NextPM::transcodeData(PM_DATA & data)
{
	data.PM_PPM_10_0 = sub_data.PM_PPM_10_0;
	data.PM_PPM_1_0 = sub_data.PM_PPM_1_0;
	data.PM_PPM_2_5 = sub_data.PM_PPM_2_5;

	data.PM_UG_10_0 = sub_data.PM_UG_10_0;
	data.PM_UG_2_5 = sub_data.PM_UG_2_5;
	data.PM_UG_1_0 = sub_data.PM_UG_1_0;

}

bool NextPM::readTempHumi(float & temp, float & humidity)
{
	byte message [] = {0x81, 0x14, 0x6B};
	this->hstream->write(message, sizeof(message));
	delay(NEXT_PM_COMMAND_DELAY);
	if(this->readData()== TH_MEASURE)
	{
		temp= this->temperature;
		humidity = this->humidity;
		return true;
	}
	temp= 0;
	humidity = 0;
	return false;
}

NextPM::NextPM(HardwareSerial& s): PM_Sensor(s)
{
	this->hstream = &s;
	converter.array[0]=0;
	converter.array[1]=0;
	converter.array[2]=0;
	converter.array[3]=0;
	this->humidity = 0;
	this->temperature = 0;
}

NextPM::~NextPM() {
	// TODO Auto-generated destructor stub
}

NextPM::CommandResponse NextPM::readData()
{
	int hexIn = 0;
	int is_read = 0;
	unsigned int start = millis();
	delay(500);
	do
	{

	  int nbAvailable = hstream->available();
	  if( nbAvailable >3)
	  {
		  int index = 0;
		  byte mbuffer [nbAvailable];

		  int lr = hstream->readBytes(mbuffer, nbAvailable);

		  if(!checkCRC(mbuffer,nbAvailable))
		  {
			  Serial.write("crc error ");
			  return CRC_ERROR;
		  }

		  hexIn = mbuffer[1];
		  Serial.println(hexIn,HEX);
		  Serial.println(hexIn);

		 switch(hexIn)
		  {
			case 0x11: {Serial.println("10sec");readPM(mbuffer,nbAvailable); return PM_MEASURE;}
			case 0x12: {Serial.println("60sec");readPM(mbuffer,nbAvailable); return PM_MEASURE;}
			case 0x13: {Serial.println("1min");readPM(mbuffer,nbAvailable); return PM_MEASURE;}
			case 0x14: {readTempHumidity(mbuffer,nbAvailable); return TH_MEASURE;}
			case 0x15: {return is_sleeping?STOP_MEASURE:START_MEASURE;}
			case 0x16: {readSTATE(mbuffer,nbAvailable); return SENSOR_STATE;}
		  }
	  }

	} while(is_read == 0 && millis() - start < 2000);
	return TIME_OUT_ERROR;
}

void NextPM::readSTATE(byte * data, int ml)
{
	Serial.println("****************************************");
	Serial.println(data[3],BIN);
	Serial.println("****************************************");
		is_sleeping  = (data[3] & 0b00000001) ==0;
	Serial.print ("sleeping state ");
	Serial.println(is_sleeping);
	is_degraded = (data[3] & 0b00000010) ==1;
	Serial.print ("degraded state ");
	Serial.println(is_degraded);
	bool is_ready = (data[3] & 0b00000100) ==0;
	Serial.print ("ready state ");
	Serial.println(is_ready);
	bool is_temp_ok  = (data[3] & 0b00001000) ==0;
	Serial.print("check c°");
	Serial.println(is_temp_ok);
	bool is_TRX_ok  = (data[3] & 0b00010000) ==0;
	Serial.print ("TRX state ");
	Serial.println(is_TRX_ok);
	bool is_fan_ok  = (data[3] & 0b00100000) ==0;
	Serial.print ("FAN state ");
	Serial.println(is_fan_ok);
	bool is_memory_ok = (data[3] & 0b01000000) ==0;
	Serial.print ("memory state ");
	Serial.println(is_memory_ok);
	bool is_laser_ok = (data[3] & 0b10000000) ==0;
	Serial.print ("laser state ");
	Serial.println(is_laser_ok);
}

void NextPM::readTempHumidity(byte * data, int ml)
{
  converter.array[1]=data[3];
  converter.array[0]=data[4];
//calcul de la température
  this->temperature = (0.9754*(converter.integer/100.0)) - 4.2488 ;
  //calcul de l'humidité
  converter.array[1]=data[5];
  converter.array[0]=data[6];
  this->humidity = 1.1768*(converter.integer/100.0) - 4.727;
}

void NextPM::readPM(byte * data, int ml)
{
  converter.array[1]=data[3];
  converter.array[0]=data[4];
  sub_data.PM_PPM_1_0 = converter.integer;

  converter.array[1]=data[5];
  converter.array[0]=data[6];
  sub_data.PM_PPM_2_5=converter.integer;

  converter.array[1]=data[7];
  converter.array[0]=data[8];
  sub_data.PM_PPM_10_0=converter.integer;

  converter.array[1]=data[9];
  converter.array[0]=data[10];
  sub_data.PM_UG_1_0 = converter.integer * 0.1;

  converter.array[1]=data[11];
  converter.array[0]=data[12];
  sub_data.PM_UG_2_5 = converter.integer * 0.1;

  converter.array[1]=data[13];
  converter.array[0]=data[14];
  sub_data.PM_UG_10_0 = converter.integer * 0.1;
}


bool NextPM::checkCRC(byte * buf, int len)
{
  if(len<2)
    return false;
  uint16_t c1 = this->CRC(buf, len-1);
  return c1 == buf[len-1];
}

uint16_t NextPM::CRC(byte * buf, int len)
{
  uint16_t res = 0x00;
  for (int i = 0; i < len; i++) {     // Loop over each bit
    res = res + buf[i];
  }
  uint16_t res2 = 0x100 - res%256;

  return res2;
}



