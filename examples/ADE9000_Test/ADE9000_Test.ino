// Required libraries
#include <ADE9000.h>

ADE9000 ade;

void setup()
{
  delay(2000); //give some time for everything to come up (USB takes time)
  Serial.begin(115200);
  //8Mhz SPI speed (20Mhz is max), chip select on pin 25
  ade.SPI_Init(8000000, 25);
  ade.SetupADE9000(); 
  ade.L1ICal(10.0); //set these to the proper values
  ade.L2ICal(10.0); //easier said than done probably.
  ade.L1VCal(10.0); //But, run program, measure values externally, calibrate, retry.
  ade.L2VCal(10.0);
  Serial.println("Set up finished");
}

void loop()
{
  delay(1000);
  Serial.print("Phase A Current: ");
  Serial.println(ade.L1I());
  Serial.print("Phase B Current: ");
  Serial.println(ade.L2I());

  Serial.print("Phase A Voltage: ");
  Serial.println(ade.L1Vrms());
  Serial.print("Phase B Voltage: ");
  Serial.println(ade.L2Vrms());
}