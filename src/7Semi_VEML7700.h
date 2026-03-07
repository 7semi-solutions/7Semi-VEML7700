#ifndef _7SEMI_VEML7700_H
#define _7SEMI_VEML7700_H

#include <Arduino.h>
#include <Wire.h>

/**
 * VEML7700 default I2C address
 */
#define VEML7700_I2C_ADDR 0x10

/**
 * VEML7700 register map
 *
 * - All registers are 16-bit
 */
#define VEML7700_ALS_CONF_0    0x00
#define VEML7700_ALS_WH        0x01
#define VEML7700_ALS_WL        0x02
#define VEML7700_PWR_MODE      0x03
#define VEML7700_ALS           0x04
#define VEML7700_REG_WHITE     0x05
#define VEML7700_ALS_INT       0x06
#define VEML7700_REG_ID        0x07

/**
 * ALS gain settings
 *
 * - Controls sensor amplification
 * - Higher gain improves low-light sensitivity
 * - Lower gain prevents saturation in bright light
 *
 * Gain options:
 * - x1
 * - x2
 * - x1/8
 * - x1/4
 */
enum VEML7700_Gain
{
    VEML7700_GAIN_1     = 0, // gain = x1
    VEML7700_GAIN_2     = 1, // gain = x2
    VEML7700_GAIN_0_125 = 2, // gain = x3
    VEML7700_GAIN_0_25  = 3  // gain = x4
};

/**
 * ALS integration time settings
 *
 * - Defines sensor sampling duration
 * - Longer integration improves low-light resolution
 * - Shorter integration avoids saturation in bright light
 *
 * Integration times:
 * - 25 ms
 * - 50 ms
 * - 100 ms (default)
 * - 200 ms
 * - 400 ms
 * - 800 ms
 */
enum VEML7700_IntegrationTime
{
    VEML7700_IT_25MS  = 12,// 25 ms
    VEML7700_IT_50MS  = 8, // 50 ms
    VEML7700_IT_100MS = 0, // 100 ms
    VEML7700_IT_200MS = 1, // 200 ms
    VEML7700_IT_400MS = 2, // 400 ms
    VEML7700_IT_800MS = 3  // 800 ms
};

/**
 * ALS persistence protection
 *
 * - Defines number of consecutive measurements
 *   required before interrupt triggers
 *
 * Example:
 * - persistence = 4 → interrupt after 4 threshold events
 */
enum VEML7700_Persistence
{
    VEML7700_PERS_1 = 1,
    VEML7700_PERS_2 = 1,
    VEML7700_PERS_4 = 2,
    VEML7700_PERS_8 = 3
};

/**
 * Power saving modes
 *
 * - Reduces power consumption between measurements
 * - Longer refresh times reduce current consumption
 */
enum VEML7700_PwrMode
{
    VEML7700_PSM_MODE1 = 0,
    VEML7700_PSM_MODE2 = 1,
    VEML7700_PSM_MODE3 = 2,
    VEML7700_PSM_MODE4 = 3
};

/**
 * VEML7700 Ambient Light Sensor Driver
 *
 * - Provides high-level control of the VEML7700 ALS sensor
 * - Supports configuration, lux measurement and white channel data
 * - Uses I2C communication via Arduino Wire library
 */
class VEML7700_7Semi
{
public:

    /**
     * Initialize VEML7700 sensor
     *
     * - Starts I2C communication
     * - Verifies device presence
     * - Applies default configuration
     *
     * - Return
     *   - true  → sensor initialized
     *   - false → device not detected
     */
    bool begin(uint8_t i2cAddress = 0x10,
               TwoWire &i2cPort = Wire,
               uint32_t i2cClock = 400000,
               uint8_t sda = 255,
               uint8_t scl = 255);

    /**
     * Read device ID register
     *
     * - Returns 16-bit sensor ID value
     */
    bool getDeviceID(uint16_t &id);

    /**
     * Configure ALS gain
     *
     * - Adjusts sensitivity of ambient light measurement
     */
    bool setGain(VEML7700_Gain gain);

    /**
     * Read current ALS gain setting
     */
    bool getGain(uint8_t &gain);

    /**
     * Configure ALS integration time
     *
     * - Determines measurement duration
     */
    bool setIntegrationTime(VEML7700_IntegrationTime integrationTime);

    /**
     * Read integration time setting
     */
    bool getIntegrationTime(uint8_t &integrationTime);

    /**
     * Automatic measurement range adjustment
     *
     * - Dynamically adjusts gain and integration time
     * - Prevents saturation in bright light
     * - Improves resolution in dark environments
     */
    bool autoRange();

    /**
     * Configure interrupt persistence
     */
    bool setPersistenceProtect(VEML7700_Persistence readings);

    /**
     * Read interrupt persistence setting
     */
    bool getPersistenceProtect(uint8_t &readings);

    /**
     * Enable or disable ALS interrupt
     */
    bool setInterrupt(bool enable);

    /**
     * Read interrupt enable status
     */
    bool getInterrupt(bool &enable);

    /**
     * Control sensor power state
     *
     * - true  → sensor active
     * - false → shutdown mode
     */
    bool powerOn(bool enable = true);

    /**
     * Reset sensor configuration
     *
     * - Restores default configuration
     */
    bool reset();

    /**
     * Configure ALS high threshold
     */
    bool setThresholdHigh(uint16_t thHigh);

    /**
     * Read ALS high threshold
     */
    bool getThresholdHigh(uint16_t &thHigh);

    /**
     * Configure ALS low threshold
     */
    bool setThresholdLow(uint16_t thLow);

    /**
     * Read ALS low threshold
     */
    bool getThresholdLow(uint16_t &thLow);

    /**
     * Configure power saving mode
     */
    bool setPowerMode(VEML7700_PwrMode mode, bool enable);

    /**
     * Read power saving configuration
     */
    bool getPowerMode(uint8_t &mode, bool &enable);

    /**
     * Read raw ALS measurement
     *
     * - Returns raw ADC value from ambient light sensor
     */
    bool readRawALS(uint16_t &rawALS);

    /**
     * Read calculated lux value
     *
     * - Converts ALS raw value into lux
     */
    bool readLux(float &lux);

    /**
     * Read raw white channel data
     */
    bool readRawWhite(uint16_t &rawWhite);

    /**
     * Read white light level
     *
     * - Broadband light measurement
     */
    bool getWhiteLevel(float &whiteLevel);

    /**
     * Read interrupt status flags
     *
     * - Indicates if thresholds were exceeded
     */
    bool readInterruptStatus(bool &thLow, bool &thHigh);

    bool readReg(uint8_t reg, uint16_t &value);


private:

    /**
     * Internal I2C interface reference
     */
    TwoWire *i2c = nullptr;

    /**
     * Sensor I2C address
     */
    uint8_t address = 0x10;

    /**
     * Gain scaling factor
     */
    float lux_gain = 1.0f;

    /**
     * Lux conversion factor
     */
    float lux_per_lsb = 0.0336f;

    /**
     * Current measurement resolution
     */
    float resolution = 0.0336f;

    // uint16_t  last_raw_lux = 0;
    // uint16_t  last_raw_wl = 0;

    bool writeReg(uint8_t reg, uint16_t value);
    // bool readReg(uint8_t reg, uint16_t &value);

    bool writeBits(uint8_t reg, uint8_t pos, uint8_t len, uint16_t value);
    bool readBits(uint8_t reg, uint8_t pos, uint8_t len, uint16_t &value);

    bool writeBit(uint8_t reg, uint8_t pos, bool value);
    bool readBit(uint8_t reg, uint8_t pos, bool &value);
};

#endif