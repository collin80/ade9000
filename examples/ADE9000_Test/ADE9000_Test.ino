// Required libraries
#include <ADE9000.h>

ADE9000 ade;

void setup()
{
  delay(2000); //give some time for everything to come up
  Serial.begin(115200);
  //8Mhz SPI speed (20Mhz is max), chip select on pin 25
  //Note 20MHz doesn't seem to test to work. You have been warned! Leave it at 8Mhz and you'll be happy
  ade.SPI_Init(8000000, 25);
  ade.begin(); //set up the chip and get it running
  ade.L1ICal(0.00000235); //set these to the proper values
  ade.L2ICal(0.00000235); //easier said than done probably.
  ade.L1VCal(0.00000988); //But, run program, measure values externally, calibrate, retry.
  ade.L2VCal(0.00000988);
  ade.L1PCal(0.003449712); //Calibration for power - watts, VA, VAR
  ade.L2PCal(0.000001);
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

  Serial.print("Phase A Apparent VoltAmps: ");
  Serial.println(ade.L1VA());
  Serial.print("Phase B Apparent VoltAmps: ");
  Serial.println(ade.L2VA());
  
  Serial.print("Phase A Reactive VoltAmps: ");
  Serial.println(ade.L1VAR());
  Serial.print("Phase B Reactive VoltAmps: ");
  Serial.println(ade.L2VAR());

  Serial.print("Phase A Watts: ");
  Serial.println(ade.L1Watt());
  Serial.print("Phase B Watts: ");
  Serial.println(ade.L2Watt());

  Serial.print("Frequency: ");
  Serial.println(ade.frequency());
  Serial.print("Power Factor: ");
  Serial.print(ade.PF());
  Serial.println("%");
  Serial.print("Total Harmonic Distortion: ");
  Serial.print(ade.THD());
  Serial.println("%");

  Serial.println();
  Serial.println();
}