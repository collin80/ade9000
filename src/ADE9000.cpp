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
		m_flipCurr = false;
}

void ADE9000::loadParams()
{
    preferences.begin("ADE9000", false);
		/*  
		//I don't know that we want to mess with these. Maybe just have our own conversion factors
		//Also, the hardware stores these in integer format so maybe store them that way instead.
    OverVSetpoint = preferences.getFloat("AIGain", 1.0f);
		OverVSetpoint = preferences.getFloat("BIGain", 1.0f);
		OverVSetpoint = preferences.getFloat("CIGain", 1.0f);
		OverVSetpoint = preferences.getFloat("NIGain", 1.0f);
		OverVSetpoint = preferences.getFloat("AVGain", 1.0f);
		OverVSetpoint = preferences.getFloat("BVGain", 1.0f);
		OverVSetpoint = preferences.getFloat("CVGain", 1.0f);
		OverVSetpoint = preferences.getFloat("APHCal0", 1.0f);
		OverVSetpoint = preferences.getFloat("BPHCal0", 1.0f);
		OverVSetpoint = preferences.getFloat("CPHCal0", 1.0f);
		OverVSetpoint = preferences.getFloat("APGain", 1.0f);
		OverVSetpoint = preferences.getFloat("BPGain", 1.0f);
		OverVSetpoint = preferences.getFloat("CPGain", 1.0f);
		*/
		m_L1vcal_p = preferences.getFloat("L1VCal", 1.0f);
		m_L1vcal_n = preferences.getFloat("L1VCalNeg", m_L1vcal_p);
		m_L2vcal_p = preferences.getFloat("L2VCal", 1.0f);
		m_L2vcal_n = preferences.getFloat("L2VCalNeg", m_L2vcal_p);
		m_L1ical_p = preferences.getFloat("L1ICal", 1.0f);
		m_L1ical_n = preferences.getFloat("L1ICalNeg", m_L1ical_p);
		m_L2ical_p = preferences.getFloat("L2ICal", 1.0f);
		m_L2ical_n = preferences.getFloat("L2ICalNeg", m_L2ical_p);
		m_L1pcal_p = preferences.getFloat("L1PCal", 1.0f);
		m_L1pcal_n = preferences.getFloat("L1PCalNeg", m_L1pcal_p);
		m_L2pcal_p = preferences.getFloat("L2PCal", 1.0f);
		m_L2pcal_n = preferences.getFloat("L2PCalNeg", m_L2pcal_p);
		KWH = preferences.getFloat("KWH", 1.0f);
 
    preferences.end();
}

void ADE9000::saveParams()
{
    preferences.begin("ADE9000", false);
		/*
    preferences.putFloat("AIGain", 1.0f);
		preferences.putFloat("BIGain", 1.0f);
		preferences.putFloat("CIGain", 1.0f);
		preferences.putFloat("NIGain", 1.0f);
		preferences.putFloat("AVGain", 1.0f);
		preferences.putFloat("BVGain", 1.0f);
		preferences.putFloat("CVGain", 1.0f);
		preferences.putFloat("APHCal0", 1.0f);
		preferences.putFloat("BPHCal0", 1.0f);
		preferences.putFloat("CPHCal0", 1.0f);
		preferences.putFloat("APGain", 1.0f);
		preferences.putFloat("BPGain", 1.0f);
		preferences.putFloat("CPGain", 1.0f);
		*/
		preferences.putFloat("L1VCal", m_L1vcal_p);
		preferences.putFloat("L2VCal", m_L2vcal_p);
		preferences.putFloat("L1ICal", m_L1ical_p);
		preferences.putFloat("L2ICal", m_L2ical_p);
		preferences.putFloat("L1PCal", m_L1pcal_p);
		preferences.putFloat("L2PCal", m_L2pcal_p);
		preferences.putFloat("L1VCalNeg", m_L1vcal_n);
		preferences.putFloat("L2VCalNeg", m_L2vcal_n);
		preferences.putFloat("L1ICalNeg", m_L1ical_n);
		preferences.putFloat("L2ICalNeg", m_L2ical_n);
		preferences.putFloat("L1PCalNeg", m_L1pcal_n);
		preferences.putFloat("L2PCalNeg", m_L2pcal_n);
		preferences.putFloat("KWH", KWH);

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
	 SPI_Write_16(ADDR_WFB_CFG,ADE9000_WFB_CFG);
	 SPI_Write_32(ADDR_VLEVEL,ADE9000_VLEVEL);
	 SPI_Write_32(ADDR_DICOEFF,ADE9000_DICOEFF);
	 SPI_Write_16(ADDR_EGY_TIME,ADE9000_EGY_TIME);
	 SPI_Write_16(ADDR_EP_CFG,ADE9000_EP_CFG);		//Energy accumulation ON
	 SPI_Write_16(ADDR_RUN,ADE9000_RUN_ON);		//DSP ON
	 loadParams();
}

void ADE9000::flipCurrentDirection(bool flip)
{
	m_flipCurr = flip;
}

//instantaneous current on phase A (rms current)
float ADE9000::L1I()
{
	float outVal;
	int32_t valu = int32_t (SPI_Read_32(ADDR_AIRMS)); //Get rms current for phase A
	if(SPI_Read_16(ADDR_PHSIGN)&1)valu*=-1;  //If bit 0 of sign register value is negative
	if (m_flipCurr) valu *= -1;
	if (valu < 0) outVal = valu * m_L1ical_n;			  //Apply calibration factor
	else outVal = valu * m_L1ical_p;
	return outVal;
}

//instantaneous current on phase B (rms current)
float ADE9000::L2I()
{
	float outVal;
	int32_t valu = int32_t (SPI_Read_32(ADDR_BIRMS)); //Get rms current for phase A
	if(SPI_Read_16(ADDR_PHSIGN)&4)valu*=-1;  //If bit 0 of sign register value is negative
	if (m_flipCurr) valu *= -1;
	if (valu < 0) outVal = valu * m_L2ical_n;			  //Apply calibration factor
	else outVal = valu * m_L2ical_p;
	return outVal;
}

//instantaneous rms voltage on phase A
float ADE9000::L1Vrms()
{
	float outVal;
	bool negCurr = SPI_Read_16(ADDR_PHSIGN)&1;
	if (m_flipCurr) negCurr = !negCurr;
	int32_t valu = int32_t (SPI_Read_32(ADDR_AVRMS));
#ifdef DEBUGADE
	Serial.print("AVRMS ");
	Serial.println(valu, HEX);
#endif
	if (negCurr) outVal = valu * m_L1vcal_n;
	else outVal = valu * m_L1vcal_p;
	return outVal;
}

//instantaneous rms voltage on phase B
float ADE9000::L2Vrms()
{
	float outVal;
	bool negCurr = SPI_Read_16(ADDR_PHSIGN)&4;
	if (m_flipCurr) negCurr = !negCurr;
	int32_t valu = int32_t (SPI_Read_32(ADDR_BVRMS));
#ifdef DEBUGADE
	Serial.print("BVRMS ");
	Serial.println(valu, HEX);
#endif
	if (negCurr) outVal = valu * m_L2vcal_n;
	else outVal = valu * m_L2vcal_p;
	return outVal;
}

//instantaneous wattage on phase A
float ADE9000::L1Watt()
{
	float outVal;
	int32_t valu = int32_t (SPI_Read_32(ADDR_AWATT));
#ifdef DEBUGADE
	Serial.print("AWATT ");
	Serial.println(valu, HEX);
#endif
	if (m_flipCurr) valu *= -1;
	if (valu < 0) outVal = valu * m_L1pcal_n;
	else outVal = valu * m_L1pcal_p;

	return outVal;
}

//instantaneous wattage on phase B
float ADE9000::L2Watt()
{
	float outVal;
	int32_t valu = int32_t (SPI_Read_32(ADDR_BWATT));
#ifdef DEBUGADE
	Serial.print("AWATT ");
	Serial.println(valu, HEX);
#endif
	if (m_flipCurr) valu *= -1;
	if (valu < 0) outVal = valu * m_L2pcal_n;
	else outVal = valu * m_L2pcal_p;
	
	return outVal;
}

//total wattage of A and B together
float ADE9000::Watt()
{
	return L1Watt() + L2Watt();
}

//instantaneous apparent power in volt amps on phase A
float ADE9000::L1VA()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_AVA));
#ifdef DEBUGADE
	Serial.print("AVA ");
	Serial.println(valu, HEX);
#endif
	float outVal = valu * m_L1pcal_p;
	return outVal;
}

//instantaneous apparent power in volt amps on phase B
float ADE9000::L2VA()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_BVA));
#ifdef DEBUGADE
	Serial.print("BVA ");
	Serial.println(valu, HEX);
#endif
	float outVal = valu * m_L2pcal_p;
	return outVal;
}

float ADE9000::L1VAR()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_AVAR));
#ifdef DEBUGADE
	Serial.print("AVAR ");
	Serial.println(valu, HEX);
#endif
	float outVal = valu * m_L1pcal_p;
	return outVal;
}

float ADE9000::L2VAR()
{
	int32_t valu = int32_t (SPI_Read_32(ADDR_BVAR));
#ifdef DEBUGADE
	Serial.print("BVAR ");
	Serial.println(valu, HEX);
#endif
	float outVal = valu * m_L1pcal_p;
	return outVal;
}

//total volt amps of phase A and B together
float ADE9000::VA()
{
	return L1VA() + L2VA();
}

//line frequency of the system (measured at phase A)
float ADE9000::frequency()
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
float ADE9000::THD()
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
float ADE9000::PF()
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
void ADE9000::L1VCal(float calFactor)
{
	m_L1vcal_p = calFactor;
	m_L1vcal_n = calFactor;
}

//voltage gain factor to turn reading into actual voltage - Phase A - Positive Current Flow 
void ADE9000::L1VCalPos(float calFactor)
{
	m_L1vcal_p = calFactor;
}

//voltage gain factor to turn reading into actual voltage - Phase A - Negative Current Flow
void ADE9000::L1VCalNeg(float calFactor)
{
	m_L1vcal_n = calFactor;
}

//get factor for Phase A
float ADE9000::L1VCal()
{
	return m_L1vcal_p;
} 

//get factor for Phase A
float ADE9000::L1VCalPos()
{
	return m_L1vcal_p;
}

//get factor for Phase A
float ADE9000::L1VCalNeg()
{
	return m_L1vcal_n;
}

//voltage gain factor to turn reading into actual voltage - Phase B
void ADE9000::L2VCal(float calFactor)
{
	m_L2vcal_p = calFactor;
	m_L2vcal_n = calFactor;
}

//voltage gain factor to turn reading into actual voltage - Phase B - Positive Current Flow 
void ADE9000::L2VCalPos(float calFactor)
{
	m_L2vcal_p = calFactor;
}

//voltage gain factor to turn reading into actual voltage - Phase B - Negative Current Flow
void ADE9000::L2VCalNeg(float calFactor)
{
	m_L2vcal_n = calFactor;
}

//get factor for Phase B 
float ADE9000::L2VCal()
{
	return m_L2vcal_p;
}

//get factor for Phase B
float ADE9000::L2VCalPos()
{
	return m_L2vcal_p;
}

//get factor for Phase B
float ADE9000::L2VCalNeg()
{
	return m_L2vcal_n;
}

//current gain factor to turn reading into actual current - Phase A
void ADE9000::L1ICal(float calFactor)
{
	m_L1ical_p = calFactor;
	m_L1ical_n = calFactor;
}

//current gain factor to turn reading into actual current - Phase A - Positive Current Flow 
void ADE9000::L1ICalPos(float calFactor)
{
	m_L1ical_p = calFactor;
}

//current gain factor to turn reading into actual current - Phase A - Negative Current Flow
void ADE9000::L1ICalNeg(float calFactor)
{
	m_L1ical_n = calFactor;
}

//get factor for Phase A
float ADE9000::L1ICal()
{
	return m_L1ical_p;
} 

//get factor for Phase A
float ADE9000::L1ICalPos()
{
	return m_L1ical_p;
}

//get factor for Phase A
float ADE9000::L1ICalNeg()
{
	return m_L1ical_n;
}

//current gain factor to turn reading into actual current - Phase B
void ADE9000::L2ICal(float calFactor)
{
	m_L2ical_p = calFactor;
	m_L2ical_n = calFactor;
}

//current gain factor to turn reading into actual current - Phase B - Positive Current Flow 
void ADE9000::L2ICalPos(float calFactor)
{
	m_L2ical_p = calFactor;
}

//current gain factor to turn reading into actual current - Phase B - Negative Current Flow
void ADE9000::L2ICalNeg(float calFactor)
{
	m_L2ical_n = calFactor;
}

//get factor for Phase B 
float ADE9000::L2ICal()
{
	return m_L2ical_p;
}

//get factor for Phase B
float ADE9000::L2ICalPos()
{
	return m_L2ical_p;
}

//get factor for Phase B
float ADE9000::L2ICalNeg()
{
	return m_L2ical_n;
}

//power gain factor to turn reading into actual power - Phase A
void ADE9000::L1PCal(float calFactor)
{
	m_L1pcal_p = calFactor;
	m_L1pcal_n = calFactor;
}

//power gain factor to turn reading into actual power - Phase A - Positive Current Flow 
void ADE9000::L1PCalPos(float calFactor)
{
	m_L1pcal_p = calFactor;
}

//power gain factor to turn reading into actual power - Phase A - Negative Current Flow
void ADE9000::L1PCalNeg(float calFactor)
{
	m_L1pcal_n = calFactor;
}

//get factor for Phase A
float ADE9000::L1PCal()
{
	return m_L1pcal_p;
} 

//get factor for Phase A
float ADE9000::L1PCalPos()
{
	return m_L1pcal_p;
}

//get factor for Phase A
float ADE9000::L1PCalNeg()
{
	return m_L1pcal_n;
}

//power gain factor to turn reading into actual power - Phase B
void ADE9000::L2PCal(float calFactor)
{
	m_L2pcal_p = calFactor;
	m_L2pcal_n = calFactor;
}

//power gain factor to turn reading into actual power - Phase B - Positive Current Flow 
void ADE9000::L2PCalPos(float calFactor)
{
	m_L2pcal_p = calFactor;
}

//power gain factor to turn reading into actual power - Phase B - Negative Current Flow
void ADE9000::L2PCalNeg(float calFactor)
{
	m_L2pcal_n = calFactor;
}

//get factor for Phase B 
float ADE9000::L2PCal()
{
	return m_L2pcal_p;
}

//get factor for Phase B
float ADE9000::L2PCalPos()
{
	return m_L2pcal_p;
}

//get factor for Phase B
float ADE9000::L2PCalNeg()
{
	return m_L2pcal_n;
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

/* 
Description: Burst reads the content of waveform buffer. This function only works with resampled data. Configure waveform buffer to have Resampled data, and burst enabled (BURST_CHAN=0000 in WFB_CFG Register).
Input: The starting address. Use the starting address of a data set. e.g 0x800, 0x804 etc to avoid data going into incorrect arrays. 
       Read_Element_Length is the number of data sets to read. If the starting address is 0x800, the maximum sets to read are 512.
Output: Resampled data returned in structure
*/
void ADE9000:: SPI_Burst_Read_Resampled_Wfb(uint16_t Address, uint16_t Read_Element_Length, ResampledWfbData *ResampledData)
{
	uint16_t temp;
	uint16_t i;
 

	digitalWrite(_chipSelect_Pin, LOW);
  
	SPI.transfer16(((Address << 4) & 0xFFF0)+8);  //Send the starting address
 
  //burst read the data upto Read_Length 
	for(i=0;i<Read_Element_Length;i++) 
		{
		  ResampledData->IA_Resampled[i] =  SPI.transfer16(0);
		  ResampledData->VA_Resampled[i] =  SPI.transfer16(0);
		  ResampledData->IB_Resampled[i] =  SPI.transfer16(0);
		  ResampledData->VB_Resampled[i] =  SPI.transfer16(0);
		  ResampledData->IC_Resampled[i] =  SPI.transfer16(0);
		  ResampledData->VC_Resampled[i] =  SPI.transfer16(0);
		  ResampledData->IN_Resampled[i] =  SPI.transfer16(0);
		}
	digitalWrite(_chipSelect_Pin, HIGH);
}

/* 
Description: Reads the Active power registers AWATT,BWATT and CWATT
Input: Structure name
Output: Active power codes stored in respective structure
*/
void ADE9000:: ReadActivePowerRegs(ActivePowerRegs *Data)
{
	Data->ActivePowerReg_A = int32_t (SPI_Read_32(ADDR_AWATT));
	Data->ActivePowerReg_B = int32_t (SPI_Read_32(ADDR_BWATT));
	Data->ActivePowerReg_C = int32_t (SPI_Read_32(ADDR_CWATT));
}

void ADE9000:: ReadReactivePowerRegs(ReactivePowerRegs *Data)
{
	Data->ReactivePowerReg_A = int32_t (SPI_Read_32(ADDR_AVAR));
	Data->ReactivePowerReg_B = int32_t (SPI_Read_32(ADDR_BVAR));
	Data->ReactivePowerReg_C = int32_t (SPI_Read_32(ADDR_CVAR));	
}

void ADE9000:: ReadApparentPowerRegs(ApparentPowerRegs *Data)
{
	Data->ApparentPowerReg_A = int32_t (SPI_Read_32(ADDR_AVA));
	Data->ApparentPowerReg_B = int32_t (SPI_Read_32(ADDR_BVA));
	Data->ApparentPowerReg_C = int32_t (SPI_Read_32(ADDR_CVA));	
}

void ADE9000:: ReadVoltageRMSRegs(VoltageRMSRegs *Data)
{
	Data->VoltageRMSReg_A = int32_t (SPI_Read_32(ADDR_AVRMS));
	Data->VoltageRMSReg_B = int32_t (SPI_Read_32(ADDR_BVRMS));
	Data->VoltageRMSReg_C = int32_t (SPI_Read_32(ADDR_CVRMS));	
}

void ADE9000:: ReadCurrentRMSRegs(CurrentRMSRegs *Data)
{
	Data->CurrentRMSReg_A = int32_t (SPI_Read_32(ADDR_AIRMS));
	Data->CurrentRMSReg_B = int32_t (SPI_Read_32(ADDR_BIRMS));
	Data->CurrentRMSReg_C = int32_t (SPI_Read_32(ADDR_CIRMS));
	Data->CurrentRMSReg_N = int32_t (SPI_Read_32(ADDR_NIRMS));
	
}

void ADE9000:: ReadFundActivePowerRegs(FundActivePowerRegs *Data)
{
	Data->FundActivePowerReg_A = int32_t (SPI_Read_32(ADDR_AFWATT));
	Data->FundActivePowerReg_B = int32_t (SPI_Read_32(ADDR_BFWATT));
	Data->FundActivePowerReg_C = int32_t (SPI_Read_32(ADDR_CFWATT));	
}

void ADE9000:: ReadFundReactivePowerRegs(FundReactivePowerRegs *Data)
{
	Data->FundReactivePowerReg_A = int32_t (SPI_Read_32(ADDR_AFVAR));
	Data->FundReactivePowerReg_B = int32_t (SPI_Read_32(ADDR_BFVAR));
	Data->FundReactivePowerReg_C = int32_t (SPI_Read_32(ADDR_CFVAR));	
}

void ADE9000:: ReadFundApparentPowerRegs(FundApparentPowerRegs *Data)
{
	Data->FundApparentPowerReg_A = int32_t (SPI_Read_32(ADDR_AFVA));
	Data->FundApparentPowerReg_B = int32_t (SPI_Read_32(ADDR_BFVA));
	Data->FundApparentPowerReg_C = int32_t (SPI_Read_32(ADDR_CFVA));	
}

void ADE9000:: ReadFundVoltageRMSRegs(FundVoltageRMSRegs *Data)
{
	Data->FundVoltageRMSReg_A = int32_t (SPI_Read_32(ADDR_AVFRMS));
	Data->FundVoltageRMSReg_B = int32_t (SPI_Read_32(ADDR_BVFRMS));
	Data->FundVoltageRMSReg_C = int32_t (SPI_Read_32(ADDR_CVFRMS));	
}

void ADE9000:: ReadFundCurrentRMSRegs(FundCurrentRMSRegs *Data)
{
	Data->FundCurrentRMSReg_A = int32_t (SPI_Read_32(ADDR_AIFRMS));
	Data->FundCurrentRMSReg_B = int32_t (SPI_Read_32(ADDR_BIFRMS));
	Data->FundCurrentRMSReg_C = int32_t (SPI_Read_32(ADDR_CIFRMS));	
}

void ADE9000:: ReadHalfVoltageRMSRegs(HalfVoltageRMSRegs *Data)
{
	Data->HalfVoltageRMSReg_A = int32_t (SPI_Read_32(ADDR_AVRMSONE));
	Data->HalfVoltageRMSReg_B = int32_t (SPI_Read_32(ADDR_BVRMSONE));
	Data->HalfVoltageRMSReg_C = int32_t (SPI_Read_32(ADDR_CVRMSONE));	
}

void ADE9000:: ReadHalfCurrentRMSRegs(HalfCurrentRMSRegs *Data)
{
	Data->HalfCurrentRMSReg_A = int32_t (SPI_Read_32(ADDR_AIRMSONE));
	Data->HalfCurrentRMSReg_B = int32_t (SPI_Read_32(ADDR_BIRMSONE));
	Data->HalfCurrentRMSReg_C = int32_t (SPI_Read_32(ADDR_CIRMSONE));
	Data->HalfCurrentRMSReg_N = int32_t (SPI_Read_32(ADDR_NIRMSONE));
}

void ADE9000:: ReadTen12VoltageRMSRegs(Ten12VoltageRMSRegs *Data)
{
	Data->Ten12VoltageRMSReg_A = int32_t (SPI_Read_32(ADDR_AVRMS1012));
	Data->Ten12VoltageRMSReg_B = int32_t (SPI_Read_32(ADDR_BVRMS1012));
	Data->Ten12VoltageRMSReg_C = int32_t (SPI_Read_32(ADDR_CVRMS1012));	
}

void ADE9000:: ReadTen12CurrentRMSRegs(Ten12CurrentRMSRegs *Data)
{
	Data->Ten12CurrentRMSReg_A = int32_t (SPI_Read_32(ADDR_AIRMS1012));
	Data->Ten12CurrentRMSReg_B = int32_t (SPI_Read_32(ADDR_BIRMS1012));
	Data->Ten12CurrentRMSReg_C = int32_t (SPI_Read_32(ADDR_CIRMS1012));
	Data->Ten12CurrentRMSReg_N = int32_t (SPI_Read_32(ADDR_NIRMS1012));	
	
}

void ADE9000:: ReadVoltageTHDRegsnValues(VoltageTHDRegs *Data)
{
	uint32_t tempReg;
	float tempValue;
	
	tempReg=int32_t (SPI_Read_32(ADDR_AVTHD)); //Read THD register
	Data->VoltageTHDReg_A = tempReg;
	tempValue=(float)tempReg*100/(float)134217728; //Calculate THD in %
	Data->VoltageTHDValue_A=tempValue;	
	tempReg=int32_t (SPI_Read_32(ADDR_BVTHD)); //Read THD register
	Data->VoltageTHDReg_B = tempReg;
	tempValue=(float)tempReg*100/(float)134217728; //Calculate THD in %
	Data->VoltageTHDValue_B=tempValue;		
	tempReg=int32_t (SPI_Read_32(ADDR_CVTHD)); //Read THD register
	Data->VoltageTHDReg_C = tempReg;
	tempValue=(float)tempReg*100/(float)134217728; //Calculate THD in %
	Data->VoltageTHDValue_C=tempValue;			
}

void ADE9000:: ReadCurrentTHDRegsnValues(CurrentTHDRegs *Data)
{
	uint32_t tempReg;
	float tempValue;	
	
	tempReg=int32_t (SPI_Read_32(ADDR_AITHD)); //Read THD register
	Data->CurrentTHDReg_A = tempReg;
	tempValue=(float)tempReg*100/(float)134217728; //Calculate THD in %	
	Data->CurrentTHDValue_A=tempValue;		
	tempReg=int32_t (SPI_Read_32(ADDR_BITHD)); //Read THD register
	Data->CurrentTHDReg_B = tempReg;
	tempValue=(float)tempReg*100/(float)134217728; //Calculate THD in %	
	Data->CurrentTHDValue_B=tempValue;
	tempReg=int32_t (SPI_Read_32(ADDR_CITHD)); //Read THD register
	Data->CurrentTHDReg_C = tempReg;
	tempValue=(float)tempReg*100/(float)134217728; //Calculate THD in %		
	Data->CurrentTHDValue_C=tempValue;
}

void ADE9000:: ReadPowerFactorRegsnValues(PowerFactorRegs *Data)
{
	uint32_t tempReg;
	float tempValue;	
	
	tempReg=int32_t (SPI_Read_32(ADDR_APF)); //Read PF register
	Data->PowerFactorReg_A = tempReg;
	tempValue=(float)tempReg/(float)134217728; //Calculate PF	
	Data->PowerFactorValue_A=tempValue;			
	tempReg=int32_t (SPI_Read_32(ADDR_BPF)); //Read PF register
	Data->PowerFactorReg_B = tempReg;
	tempValue=(float)tempReg/(float)134217728; //Calculate PF	
	Data->PowerFactorValue_B=tempValue;	
	tempReg=int32_t (SPI_Read_32(ADDR_CPF)); //Read PF register
	Data->PowerFactorReg_C = tempReg;
	tempValue=(float)tempReg/(float)134217728; //Calculate PF	
	Data->PowerFactorValue_C=tempValue;
}

void ADE9000:: ReadPeriodRegsnValues(PeriodRegs *Data)
{
	uint32_t tempReg;
	float tempValue;	
	tempReg=int32_t (SPI_Read_32(ADDR_APERIOD)); //Read PERIOD register
	Data->PeriodReg_A = tempReg;
	tempValue=(float)(8000*65536)/(float)(tempReg+1); //Calculate Frequency	
	Data->FrequencyValue_A = tempValue;
	tempReg=int32_t (SPI_Read_32(ADDR_BPERIOD)); //Read PERIOD register
	Data->PeriodReg_B = tempReg;
	tempValue=(float)(8000*65536)/(float)(tempReg+1); //Calculate Frequency	
	Data->FrequencyValue_B = tempValue;
	tempReg=int32_t (SPI_Read_32(ADDR_CPERIOD)); //Read PERIOD register
	Data->PeriodReg_C = tempReg;
	tempValue=(float)(8000*65536)/(float)(tempReg+1); //Calculate Frequency	
	Data->FrequencyValue_C = tempValue;
}

void ADE9000:: ReadAngleRegsnValues(AngleRegs *Data)
{

	uint32_t tempReg;	
	uint16_t temp;
	float mulConstant;
	float tempValue;
	
	temp=SPI_Read_16(ADDR_ACCMODE); //Read frequency setting register
	if((temp&0x0100)>=0)
		{
			mulConstant=0.02109375;  //multiplier constant for 60Hz system
		}
	else
		{
			mulConstant=0.017578125; //multiplier constant for 50Hz system		
		}
	
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_VA_VB)); //Read ANGLE register
	Data->AngleReg_VA_VB=tempReg;
	tempValue=tempReg*mulConstant;	//Calculate Angle in degrees					
	Data->AngleValue_VA_VB=tempValue;
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_VB_VC));
	Data->AngleReg_VB_VC=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_VB_VC=tempValue;	
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_VA_VC));
	Data->AngleReg_VA_VC=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_VA_VC=tempValue;	
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_VA_IA));
	Data->AngleReg_VA_IA=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_VA_IA=tempValue;	
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_VB_IB));
	Data->AngleReg_VB_IB=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_VB_IB=tempValue;	
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_VC_IC));
	Data->AngleReg_VC_IC=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_VC_IC=tempValue;		
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_IA_IB));
	Data->AngleReg_IA_IB=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_IA_IB=tempValue;	
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_IB_IC));
	Data->AngleReg_IB_IC=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_IB_IC=tempValue;	
	tempReg=int16_t (SPI_Read_32(ADDR_ANGL_IA_IC));
	Data->AngleReg_IA_IC=tempReg;
	tempValue=tempReg*mulConstant;
	Data->AngleValue_IA_IC=tempValue;						
}

/* 
Description: Starts a new acquisition cycle. Waits for constant time and returns register value and temperature in Degree Celsius
Input:	Structure name
Output: Register reading and temperature value in Degree Celsius
*/
void ADE9000:: ReadTempRegnValue(TemperatureRegnValue *Data)
{
	uint32_t trim;
	uint16_t gain;
	uint16_t offset;
	uint16_t tempReg; 
	float tempValue;
	
	SPI_Write_16(ADDR_TEMP_CFG,ADE9000_TEMP_CFG);//Start temperature acquisition cycle with settings in defined in ADE9000_TEMP_CFG
	delay(2); //delay of 2ms. Increase delay if TEMP_TIME is changed

	trim = SPI_Read_32(ADDR_TEMP_TRIM);
	gain= (trim & 0xFFFF);  //Extract 16 LSB
	offset= ((trim>>16)&0xFFFF); //Extract 16 MSB
	tempReg= SPI_Read_16(ADDR_TEMP_RSLT);	//Read Temperature result register
	tempValue= (float)(offset>>5)-((float)tempReg*(float)gain/(float)65536); 
	
	Data->Temperature_Reg=tempReg;
	Data->Temperature=tempValue;
}


