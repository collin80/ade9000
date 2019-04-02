/*
  ADE9000API.h - Library for ADE9000/ADE9078 - Energy and PQ monitoring AFE
  Author:nchandra
  Date: 3-16-2017
*/
#ifndef ADE9000API_h
#define ADE9000API_h

/****************************************************************************************************************
 Includes
***************************************************************************************************************/

#include "Arduino.h"
#include  "ADE9000RegMap.h"

/****************************************************************************************************************
 Definitions
****************************************************************************************************************/
/*Configuration registers*/
#define ADE9000_PGA_GAIN 0x0000    	    /*PGA@0x0000. Gain of all channels=1*/
#define ADE9000_CONFIG0 0x00000000		/*Integrator disabled*/
#define ADE9000_CONFIG1	0x000E			/*CF3/ZX pin outputs Zero crossing, CF4 = DREADY */
#define ADE9000_CONFIG2	0x0C00			/*Default High pass corner frequency of 1.25Hz*/
#define ADE9000_CONFIG3	0x0000			/*Peak and overcurrent detection disabled*/
#define ADE9000_ACCMODE 0x0100			/*60Hz operation, 3P4W Wye configuration, signed accumulation*/
										/*Clear bit 8 i.e. ACCMODE=0x00xx for 50Hz operation*/
										/*ACCMODE=0x0x9x for 3Wire delta when phase B is used as reference*/	
#define ADE9000_TEMP_CFG 0x000C			/*Temperature sensor enabled*/
#define ADE9000_ZX_LP_SEL 0x0000		/*Line period and zero crossing obtained from VA only*/	
#define ADE9000_MASK0 0x00000001		/*Enable EGYRDY interrupt*/				
#define ADE9000_MASK1 0x00000000		/*MASK1 interrupts disabled*/
#define ADE9000_EVENT_MASK 0x00000000	/*Events disabled */
#define ADE9000_VLEVEL	0x0022EA28		/*Assuming Vnom=1/2 of full scale. 
										/*Refer Technical reference manual for detailed calculations.*/
#define ADE9000_DICOEFF 0x00000000 		/* Set DICOEFF= 0xFFFFE000 when integrator is enabled*/

/*Constant Definitions***/
#define ADE90xx_FDSP 8000   			/*ADE9000 FDSP: 8000sps, ADE9078 FDSP: 4000sps*/
#define ADE9000_RUN_ON 0x0001			/*DSP ON*/
/*Energy Accumulation Settings*/
#define ADE9000_EP_CFG 0x0011			/*Enable energy accumulation, accumulate samples at 8ksps*/
										/*latch energy accumulation after EGYRDY*/
										/*If accumulation is changed to half line cycle mode, change EGY_TIME*/
#define ADE9000_EGY_TIME 0x1F3F 				/*Accumulate 8000 samples*/
 
class ADE9000
{
	public:
		ADE9000();
		void begin(void); 
		
		/*SPI Functions*/
		void SPI_Init(uint32_t SPI_speed , uint8_t chipSelect_Pin);  		
		void SPI_Write_16(uint16_t Address , uint16_t Data );
		void SPI_Write_32(uint16_t Address , uint32_t Data );		
		uint16_t SPI_Read_16(uint16_t Address);
		uint32_t SPI_Read_32(uint16_t Address);

		/*Preferred public interface */
		double L1I(); //instantaneous current on phase A
		double L2I(); //instantaneous current on phase B
		double L1Vrms(); //instantaneous rms voltage on phase A
		double L2Vrms(); //instantaneous rms voltage on phase B
		double L1Watt(); //instantaneous wattage on phase A
		double L2Watt(); //instantaneous wattage on phase B
		double Watt(); //total wattage of A and B together
		double L1VA(); //apparent power in volt amps on phase A
		double L2VA(); //apparent power in volt amps on phase B
		double L1VAR(); //reactive power in volt amps on phase A
		double L2VAR(); //reactive power in volt amps on phase B
		double VA();   //total volt amps of phase A and B together
		double frequency(); //line frequency of the system (measured at phase A)
		double THD(); //total harmonic distortion, as a percentage
		double PF(); //power factor as a percentage

		void L1VCal(double calFactor); //voltage gain factor to turn reading into actual voltage - Phase A
		void L2VCal(double calFactor); //voltage gain factor to turn reading into actual voltage - Phase B
		double L1VCal(); //get factor for Phase A
		double L2VCal(); //get factor for Phase B 

		void L1ICal(double calFactor); //current gain factor to turn reading into actual current - Phase A
		void L2ICal(double calFactor); //current gain factor to turn reading into actual current - Phase B
		double L1ICal(); //get factor for Phase A
		double L2ICal(); //get factor for Phase B

		void L1PCal(double calFactor); //power gain factor to turn reading into actual wattage - Phase A
		void L2PCal(double calFactor); //power gain factor to turn reading into actual wattage - Phase B
		double L1PCal(); //get factor for Phase A
		double L2PCal(); //get factor for Phase B

		void saveParams();
		void loadParams();
		double KWH;

	private:
		uint8_t  _chipSelect_Pin;
		double m_L1vcal, m_L2vcal, m_L1ical, m_L2ical, m_L1pcal, m_L2pcal;
};

#endif