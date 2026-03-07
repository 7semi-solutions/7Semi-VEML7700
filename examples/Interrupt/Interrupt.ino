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

void setup() {
  Serial.begin(115200);

  if (!light.begin()) {
    Serial.println("Sensor not detected");
    while (1)
      ;
  }

  /**
   * Configure interrupt thresholds
   */
  light.setThresholdHigh(20000);
  light.setThresholdLow(1000);

  light.setInterrupt(true);
}

void loop() {
  float lux = 0;
  uint16_t white = 0;
  bool low = false, high = false;

  // Read ambient light
  if (!light.readLux(lux)) {
    Serial.println("Failed to read lux");
  }

  // Read white level
  if (!light.getWhiteLevel(white)) {
    Serial.println("Failed to read white level");
  }

  // Read interrupt status
  if (!light.readInterruptStatus(low, high)) {
    Serial.println("Failed to read interrupt status");
  }

  // Print values
  Serial.print("Ambient light: "); Serial.print(lux);
  Serial.print("  White Level: "); Serial.print(white);

  if (high)  Serial.print("  HIGH threshold!");
  if (low)   Serial.print("  LOW threshold!");

  Serial.println();

  // Wait before next reading
  delay(500);
}
