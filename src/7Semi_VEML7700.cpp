#include "7Semi_VEML7700.h"

/**
 * Initialize VEML7700 sensor
 *
 * - Initializes I2C interface
 * - Verifies sensor presence using I2C acknowledge
 * - Reads device ID for validation
 * - Applies default configuration
 *
 * - Default configuration
 *   - Gain = x1
 *   - Integration time = 100 ms
 *
 * - Return
 *   - true  → sensor initialized
 *   - false → device not detected
 */
bool VEML7700_7Semi::begin(uint8_t i2cAddress,
                           TwoWire &i2cPort,
                           uint32_t i2cClock,
                           uint8_t sda,
                           uint8_t scl)
{
    i2c = &i2cPort;
    address = i2cAddress;

#ifdef ARDUINO_ARCH_ESP32
    if (sda != 255 && scl != 255)
        i2c->begin(sda, scl);
    else
        i2c->begin();
#else
    (void)sda;
    (void)scl;
    i2c->begin();
#endif

    i2c->setClock(i2cClock);

    /**
     * Probe sensor address
     *
     * - Device must acknowledge I2C address
     */
    i2c->beginTransmission(address);
    if (i2c->endTransmission() != 0)
        return false;

    /**
     * Read and verify device ID
     *
     * - Device ID lower byte must equal 0x81
     */
    uint16_t id = 0;

    if (!getDeviceID(id))
        return false;

    if ((id & 0x00FF) != 0x81)
        return false;

    /**
     * Power up sensor
     */
    powerOn(true);
    delay(5);

    /**
     * Apply default measurement configuration
     */
    setGain(VEML7700_GAIN_1);
    setIntegrationTime(VEML7700_IT_100MS);

    return true;
}


/**
 * Read sensor device ID
 *
 * - Returns 16-bit device ID
 */
bool VEML7700_7Semi::getDeviceID(uint16_t &id)
{
    return readReg(VEML7700_REG_ID, id);
}

/**
 * Configure ALS gain
 *
 * - Adjusts sensitivity of ambient light measurement
 *
 * Gain scaling:
 * - x1
 * - x2
 * - x1/4
 * - x1/8
 */
bool VEML7700_7Semi::setGain(VEML7700_Gain gain)
{
    if (!writeBits(VEML7700_ALS_CONF_0, 11, 2, gain))
        return false;

    switch (gain)
    {
    case VEML7700_GAIN_1:
        lux_gain = 1.0f;
        break;

    case VEML7700_GAIN_2:
        lux_gain = 2.0f;
        break;

    case VEML7700_GAIN_0_25:
        lux_gain = 0.25f;
        break;

    case VEML7700_GAIN_0_125:
        lux_gain = 0.125f;
        break;
    }

    /**
     * Update lux conversion factor
     */
    lux_per_lsb = 0.0576 * resolution / lux_gain;

    return true;
}

/**
 * Read ALS gain configuration
 */
bool VEML7700_7Semi::getGain(uint8_t &gain)
{
    uint16_t v;

    if (!readBits(VEML7700_ALS_CONF_0, 11, 2, v))
        return false;

    gain = (uint8_t)v;
    return true;
}

/**
 * Configure ALS integration time
 *
 * - Controls measurement duration
 * - Longer integration improves low light resolution
 */
bool VEML7700_7Semi::setIntegrationTime(VEML7700_IntegrationTime integrationTime)
{
    if (!writeBits(VEML7700_ALS_CONF_0, 6, 4, integrationTime))
        return false;

    /**
     * Update sensor resolution
     */
    switch (integrationTime)
    {
    case VEML7700_IT_100MS:
        resolution = 1.0f;
        break;

    case VEML7700_IT_200MS:
        resolution = 0.5f;
        break;

    case VEML7700_IT_400MS:
        resolution = 0.25f;
        break;

    case VEML7700_IT_800MS:
        resolution = 0.125f;
        break;

    case VEML7700_IT_50MS:
        resolution = 2.0f;
        break;

    case VEML7700_IT_25MS:
        resolution = 4.0f;
        break;

    default:
        resolution = 1.0f;
    }

    lux_per_lsb = 0.0576 * resolution / lux_gain;

    return true;
}

/**
 * Read current integration time
 */
bool VEML7700_7Semi::getIntegrationTime(uint8_t &integrationTime)
{
    uint16_t v;

    if (!readBits(VEML7700_ALS_CONF_0, 6, 4, v))
        return false;

    integrationTime = (uint8_t)v;

    return true;
}


/**
 * Automatic measurement range adjustment
 *
 * - Prevents sensor saturation in bright light
 * - Improves sensitivity in dark environments
 */
bool VEML7700_7Semi::autoRange()
{
    uint16_t raw;

  if(!readRawALS(raw))
    return false;

  if(raw > 60000)
  {
    setGain(VEML7700_GAIN_0_125);
    setIntegrationTime(VEML7700_IT_50MS);
  }

  else if(raw > 30000)
  {
    setGain(VEML7700_GAIN_0_25);
    setIntegrationTime(VEML7700_IT_100MS);
  }

  else if(raw < 100)
  {
    setGain(VEML7700_GAIN_2);
    setIntegrationTime(VEML7700_IT_800MS);
  }

  else if(raw < 500)
  {
    setGain(VEML7700_GAIN_2);
    setIntegrationTime(VEML7700_IT_400MS);
  }

  return true;
}


/**
 * Configure interrupt persistence protection
 *
 * - Defines number of consecutive ALS readings
 *   required before an interrupt is triggered
 *
 * Persistence options:
 * - 1 reading
 * - 2 readings
 * - 4 readings
 * - 8 readings
 *
 * - Helps prevent false interrupt triggers caused by noise
 */
bool VEML7700_7Semi::setPersistenceProtect(VEML7700_Persistence readings)
{
  return writeBits(VEML7700_ALS_CONF_0, 4, 2, readings);
}

/**
 * Read interrupt persistence configuration
 *
 * - Returns the current persistence protection setting
 * - Indicates how many consecutive threshold events
 *   are required before interrupt activation
 */
bool VEML7700_7Semi::getPersistenceProtect(uint8_t &readings)
{
  uint16_t v;

  if (!readBits(VEML7700_ALS_CONF_0, 4, 2, v))
    return false;

  readings = static_cast<uint8_t>(v);

  return true;
}

/**
 * Enable or disable ALS interrupt
 *
 * - Controls interrupt output generation
 * - Interrupt occurs when ALS crosses configured thresholds
 *
 * Interrupt behavior:
 * - true  → interrupt enabled
 * - false → interrupt disabled
 */
bool VEML7700_7Semi::setInterrupt(bool enable)
{
  return writeBit(VEML7700_ALS_CONF_0, 1, enable);
}

/**
 * Read ALS interrupt enable status
 *
 * - Indicates whether ALS interrupt functionality
 *   is currently enabled
 */
bool VEML7700_7Semi::getInterrupt(bool &enable)
{
  return readBit(VEML7700_ALS_CONF_0, 1, enable);
}

/**
 * Control sensor power state
 *
 * - Enables or disables ambient light measurement
 *
 * Power modes:
 * - true  → sensor active (measurement enabled)
 * - false → sensor shutdown (low power mode)
 *
 * Note:
 * - Shutdown mode reduces current consumption
 */
bool VEML7700_7Semi::powerOn(bool enable)
{
  return writeBit(VEML7700_ALS_CONF_0, 0, !enable);
}

/**
 * Reset sensor configuration
 *
 * - Restores default configuration
 */
bool VEML7700_7Semi::reset()
{
  return writeReg(VEML7700_ALS_CONF_0,0x0000);
}

/**
 * Configure ALS high threshold
 *
 * - Defines upper light level limit for interrupt generation
 * - Interrupt triggers when ALS measurement exceeds threshold
 *
 * - Threshold value is 16-bit raw ALS value
 */
bool VEML7700_7Semi::setThresholdHigh(uint16_t thHigh)
{
  return writeReg(VEML7700_ALS_WH, thHigh);
}

/**
 * Read ALS high threshold value
 *
 * - Returns configured upper interrupt limit
 */
bool VEML7700_7Semi::getThresholdHigh(uint16_t &thHigh)
{
  return readReg(VEML7700_ALS_WH, thHigh);
}

/**
 * Configure ALS low threshold
 *
 * - Defines lower light level limit for interrupt generation
 * - Interrupt triggers when ALS measurement drops below threshold
 *
 * - Threshold value is 16-bit raw ALS value
 */
bool VEML7700_7Semi::setThresholdLow(uint16_t thLow)
{
  return writeReg(VEML7700_ALS_WL, thLow);
}

/**
 * Read ALS low threshold value
 *
 * - Returns configured lower interrupt limit
 */
bool VEML7700_7Semi::getThresholdLow(uint16_t &thLow)
{
  return readReg(VEML7700_ALS_WL, thLow);
}

/**
 * Configure power saving mode
 *
 * - Reduces sensor power consumption between measurements
 * - Measurement refresh time increases in deeper power modes
 *
 * Power saving modes:
 * - Mode 1
 * - Mode 2
 * - Mode 3
 * - Mode 4
 *
 * Enable behavior:
 * - true  → power saving enabled
 * - false → normal operation
 */
bool VEML7700_7Semi::setPowerMode(VEML7700_PwrMode mode, bool enable)
{
  uint8_t v = 0;

  v |= mode << 1;
  v |= enable;

  return writeBits(VEML7700_PWR_MODE, 0, 3, v & 0x7);
}

/**
 * Read power saving configuration
 *
 * - Returns active power saving mode
 * - Indicates whether power saving feature is enabled
 */
bool VEML7700_7Semi::getPowerMode(uint8_t &mode, bool &enable)
{
  uint16_t v = 0;

  if (!readBits(VEML7700_PWR_MODE, 0, 3, v))
    return false;

  mode = (v >> 1) & 0x03;
  enable = v & 0x01;

  return true;
}

/**
 * Read raw ALS measurement
 */
bool VEML7700_7Semi::readRawALS(uint16_t &rawALS)
{
    return readReg(VEML7700_ALS, rawALS);
}

/**
 * Read lux value
 *
 * - Converts raw ALS measurement into lux
 * - Includes Vishay high lux compensation
 */
bool VEML7700_7Semi::readLux(float &lux)
{
    uint16_t raw;

    if (!readRawALS(raw))
        return false;

    lux = raw * lux_per_lsb;

    /**
     * - Improves accuracy for bright environments
     * - Correction applied if lux more than 1000
     */
    if (lux > 1000)
    {
        lux = (6.0135e-13f * pow(lux, 4))
            - (9.3924e-9f * pow(lux, 3))
            + (8.1488e-5f * pow(lux, 2))
            + (1.0023f * lux);
    }

    return true;
}

/**
 * Read white light level
 *
 * - Broadband light measurement
 */
bool VEML7700_7Semi::readRagetWhiteLevelwWhite(uint16_t &rawWhite)
{
    return readReg(VEML7700_REG_WHITE, rawWhite);
}

/**
 * Read interrupt status flags
 *
 * - Indicates threshold events
 */
bool VEML7700_7Semi::readInterruptStatus(bool &thLow, bool &thHigh)
{
    uint16_t v = 0;

    if (!readBits(VEML7700_ALS_INT, 14, 2, v))
        return false;

    thLow = v & 0x02;
    thHigh = v & 0x01;

    return true;
}

/**
 * Read 16-bit value from register
 *
 * - Reads two bytes from sensor registers
 * - Combines MSB and LSB into 16 bit value
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool VEML7700_7Semi::readReg(uint8_t reg, uint16_t &value)
{
  i2c->beginTransmission(address);
  i2c->write(reg);

  if (i2c->endTransmission(false) != 0)
    return false;

  if (i2c->requestFrom(address, (size_t)2) < 2)
    return false;

  uint8_t lsb = i2c->read();
  uint8_t msb = i2c->read();
  value = (msb << 8) | lsb;

  return true;
}

/**
 * Write 16-bit value to register
 *
 * - Splits 16-bit value into MSB and LSB bytes
 * - Writes both bytes to sensor registers
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool VEML7700_7Semi::writeReg(uint8_t reg, uint16_t value)
{
  uint8_t lsb = value & 0xFF;
  uint8_t msb = (value >> 8) & 0xFF;

  i2c->beginTransmission(address);
  i2c->write(reg);

  i2c->write(lsb); // LSB first
  i2c->write(msb); // MSB second

  return (i2c->endTransmission() == 0);
}

/**
 * Write bit field to register
 *
 * - Write selected bits inside the register
 * - Other register bits remain unchanged
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool VEML7700_7Semi::writeBits(uint8_t reg, uint8_t pos, uint8_t len, uint16_t value)
{
  uint16_t v;
  if (!readReg(reg, v))
    return false;

  uint16_t mask = ((1u << len) - 1) << pos;
  v &= ~mask;
  v |= ((value << pos) & mask);

  return writeReg(reg, v);
}

/**
 * Read bit field from register
 *
 * - Read selected bits from  the register
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool VEML7700_7Semi::readBits(uint8_t reg, uint8_t pos, uint8_t len, uint16_t &value)
{
  uint16_t v;
  if (!readReg(reg, v))
    return false;
  Serial.println(v,HEX);
  uint16_t mask = ((1 << len) - 1) << pos;
  value = (uint16_t)(v & mask) >> pos;
  return true;
}

/**
 * Read bit from register
 *
 * - Read selected bit from  the register
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool VEML7700_7Semi::readBit(uint8_t reg, uint8_t pos, bool &value)
{
  uint16_t v;
  if (!readBits(reg, pos, 1, v))
    return false;

  value = v & 0x01;
  return true;
}

/**
 * Write bit to register
 *
 * - Write selected bit to  the register
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool VEML7700_7Semi::writeBit(uint8_t reg, uint8_t pos, bool value)
{
  return writeBits(reg, pos, 1, value);
}
