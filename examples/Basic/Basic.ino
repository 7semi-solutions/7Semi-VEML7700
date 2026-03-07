/**
 * ---------------------------------------------------------------------------
 * 7Semi VEML7700 Arduino Library
 *
 * Configuration Example
 *
 * Description
 * - Demonstrates configuration of sensor sensitivity
 * - Adjusts gain and integration time
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


#define I2C_ADDRESS 0x10
#define I2C_CLOCK 400000

// Arduino
#define SCL -1
#define SDA -1

// // ESP32
// #define SDA -1 // SDA 21
// #define SCL -1 // SCL 22

VEML7700_7Semi light;

void setup() {
  Serial.begin(115200);

  if (!light.begin(I2C_ADDRESS, Wire, I2C_CLOCK, SDA, SCL)) {
    Serial.println("Sensor not detected");
    while (1)
      ;
  }
  Serial.println("VEML7700 initialized successfully");
  /**
   * Configure sensor sensitivity
   */
  light.setGain(VEML7700_GAIN_2);
  light.setIntegrationTime(VEML7700_IT_400MS);
}

void loop() {
  float lux = 0;
  float white = 0;

  // Read ambient light
  if (light.readLux(lux)) {
    Serial.print("Ambient light : ");
    Serial.println(lux);
  } else {
    Serial.println("Failed to read lux");
  }

  // Read white Level
  if (light.getWhiteLevel(white)) {
    Serial.print("White Level : ");
    Serial.println(white);
  } else {
    Serial.println("Failed to read white Level");
  }
  Serial.println();
  delay(500);
}