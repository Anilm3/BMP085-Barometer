/********************************************************************************
* BMP085 Library - BMP085.h                                                     *
*                                                                               *
* Copyright (C) 2012 Anil Motilal Mahtani Mirchandani(anil.mmm@gmail.com)       *
*                                                                               *
* License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> *
* This is free software: you are free to change and redistribute it.            *
* There is NO WARRANTY, to the extent permitted by law.                         *
*                                                                               *
*********************************************************************************/

#ifndef _BMP085_H
#define _BMP085_H

#include <Arduino.h>

// All functions return -1 on error and 0 on success
class BMP085
{
	private:
		// Devices address
		static const int ADDRESS  = 0x77;
		
		// Calibration coefficients addresses
		static const int REG_AC1 = 0xAA;

		//Not really needed, since we read in one block
		//static const int REG_AC2 = 0xAC;
		//static const int REG_AC3 = 0xAE;
		//static const int REG_AC4 = 0xB0;
		//static const int REG_AC5 = 0xB2;
		//static const int REG_AC6 = 0xB4;
		//static const int REG_B1  = 0xB6;
		//static const int REG_B2  = 0xB8;
		//static const int REG_MB  = 0xBA;
		//static const int REG_MC  = 0xBC;
		//static const int REG_MD  = 0xBE;
		
		static const int REG_CMD  = 0xF4;
		static const int REG_DATA = 0xF6;
		
		static const int CMD_TEMPERATURE = 0x2E;
		static const int CMD_PRESSURE    = 0x34;
		
		// Calibration coefficients variables
		int ac1, ac2, ac3;
		unsigned int ac4, ac5, ac6;
		int b1, b2;
		long b5;
		int mb, mc, md, oss;

		int readRegister(byte reg_addr, int nbytes, byte *buffer);
		int writeRegister(byte reg_addr, int nbytes, byte *buffer);
	
		long readUTemperature();
		long readUPressure();
		
		long calculateTemperature(long utemp);
		long calculatePressure(long upressure);
		
	public:
		
		// Oversampling settings
		enum OSS_Setting
		{
			OSS_LOW = 0,
			OSS_STANDARD = 1,
			OSS_HIGH = 2,
			OSS_ULTRA_HIGHT = 3,
		};
		
		BMP085();
	
		void begin();
		void end();
	
		void read(long *temperature, long *pressure);
		
		void setSampling(OSS_Setting oversampling);
		
		long readTemperature();
		long readPressure();
		double readAltitude(double p0 = 101325.0);
		double readPressureSeaLvl(double altitude);
};

#endif // _BMP085_H
