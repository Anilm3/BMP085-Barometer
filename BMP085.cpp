/********************************************************************************
* BMP085 Library - BMP085.cpp                                                   *
*                                                                               *
* Copyright (C) 2012 Anil Motilal Mahtani Mirchandani(anil.mmm@gmail.com)       *
*                                                                               *
* License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> *
* This is free software: you are free to change and redistribute it.            *
* There is NO WARRANTY, to the extent permitted by law.                         *
*                                                                               *
*********************************************************************************/

#include <BMP085.h>
#include <Wire.h>"

int BMP085::writeRegister(byte reg_addr, int nbytes, byte *buffer)
{
	int written_bytes;
	
	Wire.beginTransmission(BMP085::ADDRESS);
	Wire.write(reg_addr);
	written_bytes = Wire.write(buffer, nbytes);
	Wire.endTransmission();
	
	return written_bytes;
}

int BMP085::readRegister(byte reg_addr, int nbytes, byte *buffer)
{
	int idx = 0;

	Wire.beginTransmission(BMP085::ADDRESS);
	Wire.write(reg_addr);
	Wire.endTransmission(); 
	
	Wire.requestFrom(BMP085::ADDRESS, nbytes);

	while(Wire.available() && idx < nbytes)
	{ 
		buffer[idx++] = Wire.read();
	}
	
	return idx;
}

BMP085::BMP085()
{
	Wire.begin();
}

void BMP085::begin()
{
	byte data_buffer[22];
	readRegister(BMP085::REG_AC1, 22, data_buffer);

	ac1 = (data_buffer[0] << 8) | data_buffer[1];
	ac2 = (data_buffer[2] << 8) | data_buffer[3];
	ac3 = (data_buffer[4] << 8) | data_buffer[5];
	ac4 = (data_buffer[6] << 8) | data_buffer[7];
	ac5 = (data_buffer[8] << 8) | data_buffer[9];
	ac6 = (data_buffer[10] << 8) | data_buffer[11];
	b1  = (data_buffer[12] << 8) | data_buffer[13];
	b2  = (data_buffer[14] << 8) | data_buffer[15];
	mb  = (data_buffer[16] << 8) | data_buffer[17];
	mc  = (data_buffer[18] << 8) | data_buffer[19];
	md  = (data_buffer[20] << 8) | data_buffer[21];
}

void BMP085::end(){}


long BMP085::readUTemperature()
{
	byte data_buffer[2];
	data_buffer[0] = BMP085::CMD_TEMPERATURE;
	
	writeRegister(BMP085::REG_CMD, 1, data_buffer);
	
	delay(5);
	
	readRegister(BMP085::REG_DATA, 2, data_buffer);
	
	return (data_buffer[0] << 8) | data_buffer[1];
}

long BMP085::readUPressure()
{
	byte data_buffer[3];
	data_buffer[0] = BMP085::CMD_PRESSURE + (oss << 6 );
	
	writeRegister(BMP085::REG_CMD, 1, data_buffer);
	
	delay(2 + (3 << oss));
	
	readRegister(BMP085::REG_DATA, 3, data_buffer);
	
	
	return (((long)data_buffer[0] << 16) | 
			 ((long)data_buffer[1] << 8)  |
			 ((long)data_buffer[2])) >> (8 - oss);
}

long BMP085::calculateTemperature(long utemp)
{
  long x1, x2;
  
  x1 = ((utemp - ac6) * ac5) >> 15;
  x2 = ((long)mc << 11) / (x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8) >> 4);   
}

long BMP085::calculatePressure(long upressure)
{
  long x1, x2, x3, b3, b6, pressure;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6) >> 12) >> 11;
  x2 = (ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = (((((long)ac1 << 2) + x3) << oss) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6) >> 13;
  x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;
  
  b7 = ((unsigned long)(upressure - b3) * (50000 >> oss));
  if (b7 < 0x80000000)
    pressure = (b7 << 1) / b4;
  else
    pressure = (b7/b4) << 1;
    
  x1 = (pressure >> 8) * (pressure >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * pressure) >> 16;
  pressure += (x1 + x2 + 3791) >> 4;
  
  return pressure;
}

void BMP085::read(long *temperature, long *pressure)
{
	long t, p;
	
	t = calculateTemperature(readUTemperature());
	p = calculatePressure(readUPressure());
	
	if (temperature == NULL) 
	{
		*temperature = t;
	}
	
	if (pressure == NULL) 
	{
		*pressure = p;
	}
}

void BMP085::setSampling(OSS_Setting oversampling)
{
	oss = oversampling;
}

long BMP085::readTemperature()
{
	return calculateTemperature(readUTemperature());
}

long BMP085::readPressure()
{
	// Get temperature for calibration
	calculateTemperature(readUTemperature());
	
	return calculatePressure(readUPressure());
}

double BMP085::readAltitude(double p0)
{
	long pressure = calculatePressure(readUPressure());
	return 44330 * (1 - pow(((double)pressure)/p0, 1.0/5.255));
}

double BMP085::readPressureSeaLvl(double altitude)
{
	long pressure = calculatePressure(readUPressure());
	return pressure / pow(1 - altitude/44330, 5.255);
}
