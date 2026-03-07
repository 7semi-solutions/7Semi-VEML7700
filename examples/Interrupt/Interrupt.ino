/**
 * ---------------------------------------------------------------------------
 * 7Semi VEML7700 Arduino Library
 *
 * Interrupt Example
 *
 * Description
 * - Demonstrates threshold interrupt configuration
 * - Triggers interrupt when light exceeds configured limits
 *
 * Connection
 *
 * VEML7700        MCU
 * -------------------------
 * VDD      →      3.3V
 * GND      →      GND
 * SDA      →      SDA
 * SCL      →      SCL
 * ---------------------------------------------------------------------------
 */

#include <7Semi_VEML7700.h>

VEML7700_7Semi light;

void setup()
{
  Serial.begin(115200);

  if(!light.begin())
  {
    Serial.println("Sensor not detected");
    while(1);
  }

  /**
   * Configure interrupt thresholds
   */
  light.setThresholdHigh(20000);
  light.setThresholdLow(100);

  light.setInterrupt(true);
}

void loop()
{
  bool low, high;

  if(light.readInterruptStatus(low, high))
  {
    if(high)
      Serial.println("High light threshold reached");

    if(low)
      Serial.println("Low light threshold reached");
  }

  delay(500);
}