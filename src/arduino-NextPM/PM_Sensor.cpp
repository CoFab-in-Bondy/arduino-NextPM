/*
 * PM_Sensor.cpp
 *
 *	(c) Copyright © 2019, 2020, 2021, IRD & Université de Bourgogne
 *  Created on: Sep 6, 2020
 *  Author: Nicolas Marilleau
 */



#include "Arduino.h"
#include <PM_Sensor.h>


PM_Sensor::PM_Sensor(HardwareSerial & s )
{
	hstream = &s;
}

bool PM_Sensor::read_1min(PM_DATA & data)
{
	PM_DATA tmp = readDuring(60000);
	data.PM_PPM_10_0 = tmp.PM_PPM_10_0;
	data.PM_PPM_1_0 = tmp.PM_PPM_1_0;
	data.PM_PPM_2_5 = tmp.PM_PPM_2_5;

	data.PM_UG_10_0 = tmp.PM_UG_10_0;
	data.PM_UG_1_0 = tmp.PM_UG_1_0;
	data.PM_UG_2_5 = tmp.PM_UG_2_5;
	return true;
}

bool PM_Sensor::read_10sec(PM_DATA & data){
	PM_DATA tmp = this->readDuring(10000);
	data.PM_PPM_10_0 = tmp.PM_PPM_10_0;
	data.PM_PPM_1_0 = tmp.PM_PPM_1_0;
	data.PM_PPM_2_5 = tmp.PM_PPM_2_5;

	data.PM_UG_10_0 = tmp.PM_UG_10_0;
	data.PM_UG_1_0 = tmp.PM_UG_1_0;
	data.PM_UG_2_5 = tmp.PM_UG_2_5;
	return true;
}

bool PM_Sensor::read_5min(PM_DATA & data){
	PM_DATA tmp = readDuring(300000);
	data.PM_PPM_10_0 = tmp.PM_PPM_10_0;
	data.PM_PPM_1_0 = tmp.PM_PPM_1_0;
	data.PM_PPM_2_5 = tmp.PM_PPM_2_5;

	data.PM_UG_10_0 = tmp.PM_UG_10_0;
	data.PM_UG_1_0 = tmp.PM_UG_1_0;
	data.PM_UG_2_5 = tmp.PM_UG_2_5;
	return true;
}

PM_DATA   PM_Sensor::readDuring(  long delay)
{
	unsigned long start = millis();
	int nb = 0;
	PM_DATA res;
	PM_DATA  data;
	PM_DATA tmp;
	res.PM_PPM_10_0=0;
	res.PM_PPM_2_5=0;
	res.PM_PPM_1_0=0;
	res.PM_UG_10_0=0;
	res.PM_UG_1_0=0;
	res.PM_UG_2_5=0;

	do
	{
		if(this->read(tmp))
		{
			res.PM_PPM_10_0+=tmp.PM_PPM_10_0;
			res.PM_PPM_2_5+=tmp.PM_PPM_2_5;
			res.PM_PPM_1_0+=tmp.PM_PPM_1_0;
			res.PM_UG_10_0+=tmp.PM_UG_10_0;
			res.PM_UG_1_0+=tmp.PM_UG_1_0;
			res.PM_UG_2_5+=tmp.PM_UG_2_5;
			nb++;
		}
		//Serial.println("millis "+millis());
	}while(millis() - start < delay );

	data.PM_PPM_10_0 = res.PM_PPM_10_0 / nb;
	data.PM_PPM_1_0 = res.PM_PPM_1_0 / nb;
	data.PM_PPM_2_5 = res.PM_PPM_2_5 / nb;
	data.PM_UG_10_0 = res.PM_UG_10_0 / nb;
	data.PM_UG_2_5 = res.PM_UG_2_5 / nb;
	data.PM_UG_1_0 = res.PM_UG_1_0 / nb;
	return data;

}
