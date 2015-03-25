/*
 * Inputs ADC Value from Thermistor and outputs Temperature in Celsius
 *  requires: include <math.h>
 * Utilizes the Steinhart-Hart Thermistor Equation:
 *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]3}
 *    where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08
 *
 * These coefficients seem to work fairly universally, which is a bit of a 
 * surprise. 
 *
 * Schematic:
 *   [Ground] -- [10k-pad-resistor] -- | -- [thermistor] --[Vcc (5 or 3.3v)]
 *                                               |
 *                                          Analog Pin 0
 *
 * In case it isn't obvious (as it wasn't to me until I thought about it), the analog ports
 * measure the voltage between 0v -> Vcc which for an Arduino is a nominal 5v, but for (say) 
 * a JeeNode, is a nominal 3.3v.
 *
 * The resistance calculation uses the ratio of the two resistors, so the voltage
 * specified above is really only required for the debugging that is commented out below
 *
 * Resistance = PadResistor * (1024/ADC -1)  
 *
 * I have used this successfully with some CH Pipe Sensors (http://www.atcsemitec.co.uk/pdfdocs/ch.pdf)
 * which be obtained from http://www.rapidonline.co.uk.
 *
 */

#include <math.h>

#define ThermistorPIN 2                 // Analog Pin 2

float pad = 9900;                       // balance/pad resistor value, set this to
                                        // the measured resistance of your pad resistor
float thermr = 10000;                   // thermistor nominal resistance

#define muxPINA 2
#define muxPINB 3
#define muxPINC 4
int muxA = 0;
int muxB = 0;
int muxC = 0;
int muxCount = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(muxPINA, OUTPUT);
  pinMode(muxPINB, OUTPUT);
  pinMode(muxPINC, OUTPUT);
}

void loop() {
  for( muxCount=0; muxCount<=7; muxCount++){
     muxA = bitRead(muxCount,0);
     muxB = bitRead(muxCount,1);
     muxC = bitRead(muxCount,2);
     
     //select mux pints
     digitalWrite(muxPINA, muxA);
     digitalWrite(muxPINB, muxB);
     digitalWrite(muxPINC, muxC);
     
     float temp;
     temp = Thermistor(analogRead(ThermistorPIN));       // read ADC and  convert it to Celsius
     Serial.print("MUX Port");
     Serial.print(muxCount);
     Serial.print("=> ");
     Serial.print("Celsius: "); 
     Serial.print(temp,3);                             // display Celsius
     Serial.println("");
  }
  Serial.println("");
  delay(2000);                                      // Delay a bit... 
}

float Thermistor(int RawADC) {
  long Resistance;  
  float Temp;  // Dual-Purpose variable to save space.

  Resistance=pad*((1024.0 / RawADC) - 1); 
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius                      

  // BEGIN- Remove these lines for the function not to display anything
  Serial.print("ADC: "); 
  Serial.print(RawADC); 
  Serial.print("/1024");                           // Print out RAW ADC Number
  Serial.print(", pad: ");
  Serial.print(pad/1000, 3);
  Serial.print(" Kohms, Resistance: ");
  Serial.print(Resistance/1000, 3);
  Serial.print(" Kohms, ");
  // END- Remove these lines for the function not to display anything

  // Uncomment this line for the function to return Fahrenheit instead.
  //temp = (Temp * 9.0)/ 5.0 + 32.0;                  // Convert to Fahrenheit
  return Temp;                                      // Return the Temperature
}
