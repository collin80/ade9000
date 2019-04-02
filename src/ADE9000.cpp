/*
  ADE9000API.cpp - Library for ADE9000/ADE9078 - Energy and PQ monitoring AFE
  Date: 3-16-2017
*/
/****************************************************************************************************************
 Includes
***************************************************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "ADE9000.h"
#include "Preferences.h"

Preferences preferences;

//uncomment next line to get debugging output from this library
//#define DEBUGADE

ADE9000::ADE9000()
{
		_chipSelect_Pin = 25;
}

void ADE9000::loadParams()
{
    preferences.begin("ADE9000", false);
		m_L1vcal = preferences.getDouble("L1VCal", 1.0f);
		m_L2vcal = preferences.getDouble("L2VCal", 1.0f);
		m_L1ical = preferences.getDouble("L1ICal", 1.0f);
		m_L2ical = preferences.getDouble("L2ICal", 1.0f);
		m_L1pcal = preferences.getDouble("L1PCal", 1.0f);
		m_L2pcal = preferences.getDouble("L2PCal", 1.0f);
		KWH = preferences.getDouble("KWH", 0.0f);
    preferences.end();
}

void ADE9000::saveParams()
{
    preferences.begin("ADE9000", false);
		preferences.putDouble("L1VCal", m_L1vcal);
		preferences.putDouble("L2VCal", m_L2vcal);
		preferences.putDouble("L1ICal", m_L1ical);
		preferences.putDouble("L2ICal", m_L2ical);
		preferences.putDouble("L1PCal", m_L1pcal);
		preferences.putDouble("L2PCal", m_L2pcal);
		preferences.putDouble("KWH", KWH);
    preferences.end();
}

/* 
Description: Initializes the ADE9000. The initial settings for registers are defined in ADE9000API.h header file
Input: Register settings in header files
Output: 
*/

void ADE9000::begin(void)
{
	 SPI_Write_16(ADDR_PGA_GAIN,ADE9000_PGA_GAIN);     
 	 SPI_Write_32(ADDR_CONFIG0,ADE9000_CONFIG0); 
	 SPI_Write_16(ADDR_CONFIG1,ADE9000_CONFIG1);
	 SPI_Write_16(ADDR_CONFIG2,ADE9000_CONFIG2);
	 SPI_Write_16(ADDR_CONFIG3,ADE9000_CONFIG3);
	 SPI_Write_16(ADDR_ACCMODE,ADE9000_ACCMODE);
	 SPI_Write_16(ADDR_TEMP_CFG,ADE9000_TEMP_CFG);
	 SPI_Write_16(ADDR_ZX_LP_SEL,ADE9000_ZX_LP_SEL);
	 SPI_Write_32(ADDR_MASK0,ADE9000_MASK0);
	 SPI_Write_32(ADDR_MASK1,ADE9000_MASK1);
	 SPI_Write_32(ADDR_EVENT_MASK,ADE9000_EVENT_MASK);
	 SPI_Write_32(ADDR_VLEVEL,ADE9000_VLEVEL);
	 SPI_Write_32(ADDR_DICOEFF,ADE9000_DICOEFF);
	 SPI_Write_16(ADDR_EGY_TIME,ADE9000_EGY_TIME);
	 SPI_Write_16(ADDR_EP_CFG,ADE9000_EP_CFG);		//Energy accumulation ON
	 SPI_Write_16(ADDR_RUN,ADE9000_RUN_ON);		//DSP ON
	 loadParams();
}

//instantaneous current on phase A (rms current)
double ADE9000::L1I()
{
	

	int32_t valu = int32_t (SPI_Read_32(ADDR_AIRMS)); //Get rms current for phase A
	double outVal = valu * m_L1ical;			  //Apply calibration factor
	if(SPI_Read_16(ADDR_PHSIGN)&1)outVal*=-1;  //If bit 0 of sign register value is negative 
	return outVal;
}

//instantaneous current on phase B (rms current)
double ADE9000::L2I()
{
	
	int32_t valu = int32_t (SPI_Read_32(ADDR_BIRMS));
	double outVal = valu * m_L2ical;
  	if(SPI_Read_16(ADDR_PHSIGN)&4)outVal*=-1;  //If bit 0 of sign register value is negativ	
	return outVal;
}

//instantaneous rms voltage on phase A
double ADE9000::L1Vrms()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_AVRMS));
#ifdef DEBUGADE
	Serial.print("AVRMS ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L1vcal;
	return outVal;
}

//instantaneous rms voltage on phase B
double ADE9000::L2Vrms()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_BVRMS));
#ifdef DEBUGADE
	Serial.print("BVRMS ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L2vcal;
	return outVal;
}

//instantaneous wattage on phase A
double ADE9000::L1Watt()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_AWATT));
#ifdef DEBUGADE
	Serial.print("AWATT ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L1pcal;
	return outVal;
}

//instantaneous wattage on phase B
double ADE9000::L2Watt()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_BWATT));
#ifdef DEBUGADE
	Serial.print("BWATT ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L2pcal;
	return outVal;	
}

//total wattage of A and B together
double ADE9000::Watt()
{
	return L1Watt() + L2Watt();
}

//instantaneous apparent power in volt amps on phase A
double ADE9000::L1VA()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_AVA));
#ifdef DEBUGADE
	Serial.print("AVA ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L1pcal;
	return outVal;
}

//instantaneous apparent power in volt amps on phase B
double ADE9000::L2VA()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_BVA));
#ifdef DEBUGADE
	Serial.print("BVA ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L2pcal;
	return outVal;
}

double ADE9000::L1VAR()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_AVAR));
#ifdef DEBUGADE
	Serial.print("AVAR ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L1pcal;
	return outVal;
}

double ADE9000::L2VAR()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_BVAR));
#ifdef DEBUGADE
	Serial.print("BVAR ");
	Serial.println(valu, HEX);
#endif
	double outVal = valu * m_L1pcal;
	return outVal;
}

//total volt amps of phase A and B together
double ADE9000::VA()
{
	return L1VA() + L2VA();
}

//line frequency of the system (measured at phase A)
double ADE9000::frequency()
{
	//frequency is (8000 * 2^16) / ((valu + 1);
	uint32_t valu = (SPI_Read_32(ADDR_APERIOD));
#ifdef DEBUGADE
	Serial.print("APERIOD ");
	Serial.println(valu, HEX);
#endif
	return 524288000.0 / (valu + 1.0);
}

//total harmonic distortion, as a percentage (Just A for now and just voltage)
double ADE9000::THD()
{
	//%THD on Current Channel A = AITHD × 2 −27 × 100%
	uint32_t valu = (SPI_Read_32(ADDR_AVTHD));
#ifdef DEBUGADE
	Serial.print("AVTHD ");
	Serial.println(valu, HEX);
#endif
	return valu / 1342177.28;
}

//power factor as a percentage (try Average of A and B power factors?)
double ADE9000::PF()
{
//Power Factor = xPF × 2 −27 * 100 to turn it to percentage
	int32_t valu = (int32_t)(SPI_Read_32(ADDR_APF));
#ifdef DEBUGADE
	Serial.print("APF ");
	Serial.println(valu, HEX);
#endif
	return valu / 1342177.28;
}

//voltage gain factor to turn reading into actual voltage - Phase A
void ADE9000::L1VCal(double calFactor)
{
	m_L1vcal = calFactor;
}

//voltage gain factor to turn reading into actual voltage - Phase B
void ADE9000::L2VCal(double calFactor)
{
	m_L2vcal = calFactor;
}

//get factor for Phase A
double ADE9000::L1VCal()
{
	return m_L1vcal;
} 

//get factor for Phase B 
double ADE9000::L2VCal()
{
	return m_L2vcal;
}

//current gain factor to turn reading into actual current - Phase A
void ADE9000::L1ICal(double calFactor)
{
	m_L1ical = calFactor;
}

//current gain factor to turn reading into actual current - Phase B
void ADE9000::L2ICal(double calFactor)
{
	m_L2ical = calFactor;
}

//get factor for Phase A
double ADE9000::L1ICal()
{
	return m_L1ical;
}

//get factor for Phase B
double ADE9000::L2ICal()
{
	return m_L2ical;
} 

//current gain factor to turn reading into actual current - Phase A
void ADE9000::L1PCal(double calFactor)
{
	m_L1pcal = calFactor;
}

//current gain factor to turn reading into actual current - Phase B
void ADE9000::L2PCal(double calFactor)
{
	m_L2pcal = calFactor;
}

//get factor for Phase A
double ADE9000::L1PCal()
{
	return m_L1pcal;
}

//get factor for Phase B
double ADE9000::L2PCal()
{
	return m_L2pcal;
} 

/* 
Description: Initializes the arduino SPI port using SPI.h library
Input: SPI speed, chip select pin
Output:-
*/
void ADE9000::SPI_Init(uint32_t SPI_speed , uint8_t chipSelect_Pin)
{
	SPI.begin();		//Initiate SPI port
	SPI.beginTransaction(SPISettings(SPI_speed,MSBFIRST,SPI_MODE0));		//Setup SPI parameters
	pinMode(chipSelect_Pin, OUTPUT);		//Set Chip select pin as output	
	digitalWrite(chipSelect_Pin, HIGH);		//Set Chip select pin high 

	_chipSelect_Pin = chipSelect_Pin;
}

/* 
Description: Writes 16bit data to a 16 bit register. 
Input: Register address, data
Output:-
*/
void ADE9000:: SPI_Write_16(uint16_t Address , uint16_t Data )
{
	uint16_t temp_address;
	
	digitalWrite(_chipSelect_Pin, LOW);
	temp_address = ((Address << 4) & 0xFFF0);	//shift address  to align with cmd packet
	SPI.transfer16(temp_address);
	SPI.transfer16(Data);
	
	digitalWrite(_chipSelect_Pin, HIGH); 	
}

/* 
Description: Writes 32bit data to a 32 bit register. 
Input: Register address, data
Output:-
*/
void ADE9000:: SPI_Write_32(uint16_t Address , uint32_t Data )
{
	uint16_t temp_address;
	uint16_t temp_highpacket;
	uint16_t temp_lowpacket;

	temp_highpacket= (Data & 0xFFFF0000)>>16;
	temp_lowpacket= (Data & 0x0000FFFF);
	
	digitalWrite(_chipSelect_Pin, LOW);
	
	temp_address = ((Address << 4) & 0xFFF0);	//shift address  to align with cmd packet
	SPI.transfer16(temp_address);
	SPI.transfer16(temp_highpacket);
	SPI.transfer16(temp_lowpacket);
	
	digitalWrite(_chipSelect_Pin, HIGH); 	
	
}

/* 
Description: Reads 16bit data from register. 
Input: Register address
Output: 16 bit data
*/
uint16_t ADE9000:: SPI_Read_16(uint16_t Address)
{
	uint16_t temp_address;
	uint16_t returnData;
	
	digitalWrite(_chipSelect_Pin, LOW);
	
	temp_address = (((Address << 4) & 0xFFF0)+8);
	SPI.transfer16(temp_address);
	returnData = SPI.transfer16(0);
	
	digitalWrite(_chipSelect_Pin, HIGH);
	return returnData;
}

/* 
Description: Reads 32bit data from register. 
Input: Register address
Output: 32 bit data
*/
uint32_t ADE9000:: SPI_Read_32(uint16_t Address)
{
	uint16_t temp_address;
	uint16_t temp_highpacket;
	uint16_t temp_lowpacket;
	uint32_t returnData;
	
	digitalWrite(_chipSelect_Pin, LOW);
	
	temp_address = (((Address << 4) & 0xFFF0)+8);
	SPI.transfer16(temp_address);
	temp_highpacket = SPI.transfer16(0);
	temp_lowpacket = SPI.transfer16(0);	
	
	digitalWrite(_chipSelect_Pin, HIGH);
	
	returnData = temp_highpacket << 16;
	returnData = returnData + temp_lowpacket;
	
	return returnData;

}

