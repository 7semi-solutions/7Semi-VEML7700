# 7Semi VEML7700 Arduino Library

Arduino driver for the **VEML7700 ambient light sensor**.

The **VEML7700** is a high-accuracy 16-bit ambient light sensor with an **I²C interface** designed for applications such as display brightness control, environmental monitoring, and smart lighting systems.

This library provides a simple interface for configuring the sensor and reading ambient light values in **lux**.

---

# Features

* Ambient light measurement (lux)
* Raw ALS data reading
* White channel light measurement
* Gain configuration
* Integration time configuration
* Interrupt threshold configuration
* Interrupt persistence protection
* Power saving modes

---

# Supported Platforms

* Arduino UNO / Mega
* ESP32
* ESP8266
* Any board supporting the **Wire (I²C) library**

---

# Hardware

Supported sensor:

**7Semi Ambient Light Sensor I2C Breakout - VEML7700**

---

# Connection

The **VEML7700 communicates using I²C**.

## I²C Connection

| VEML7700 Pin | MCU Pin | Description  |
| ------------ | ------- | ------------ |
| VDD          | 3.3V    | Sensor power |
| GND          | GND     | Ground       |
| SCL          | SCL     | I²C clock    |
| SDA          | SDA     | I²C data     |

### I²C Notes

Default sensor address:

```
0x10
```

Recommended I²C speed:

```
100 kHz – 400 kHz
```

---

# Installation

## Arduino Library Manager

1. Open **Arduino IDE**
2. Go to **Library Manager**
3. Search for **7Semi VEML7700**
4. Click **Install**

---

## Manual Installation

1. Download this repository as ZIP
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**

---

# Example

```
#include <7Semi_VEML7700.h>

VEML7700_7Semi light;

void setup()
{
  Serial.begin(115200);

  if(!light.begin())
  {
    Serial.println("VEML7700 not detected");
    while(1);
  }
}

void loop()
{
  float lux;

  if(light.readLux(lux))
  {
    Serial.print("Lux: ");
    Serial.println(lux);
  }

  delay(500);
}
```

---

# Library Overview

## Reading Ambient Light (Lux)

```
float lux;

light.readLux(lux);

Serial.print("Lux: ");
Serial.println(lux);
```

Returns ambient light value in **lux**.

---

## Reading Raw ALS Data

```
uint16_t raw;

light.readRawALS(raw);

Serial.print("ALS Raw: ");
Serial.println(raw);
```

Returns **16-bit ALS ADC value**.

---

## Reading White Channel

```
float white;

light.getWhiteLevel(white);

Serial.print("White Level: ");
Serial.println(white);
```

Returns **broadband white light measurement**.

---

# Sensor Configuration

## Gain

```
light.setGain(VEML7700_GAIN_1);
```

Available gain settings:

```
VEML7700_GAIN_1
VEML7700_GAIN_2
VEML7700_GAIN_0_25
VEML7700_GAIN_0_125
```

---

## Integration Time

```
light.setIntegrationTime(VEML7700_IT_100MS);
```

Available integration times:

```
VEML7700_IT_25MS
VEML7700_IT_50MS
VEML7700_IT_100MS
VEML7700_IT_200MS
VEML7700_IT_400MS
VEML7700_IT_800MS
```

Longer integration times improve **low-light resolution**.

---

# Automatic Range Adjustment

The driver supports **automatic gain and integration adjustment**.

```
light.autoRange();
```

This prevents **sensor saturation in bright light** and improves **low light accuracy**.

---

# Interrupt Configuration

Interrupts can be triggered when ALS measurements cross configured thresholds.

Example:

```
light.setThresholdHigh(20000);
light.setThresholdLow(100);
light.setInterrupt(true);
```

---

# Power Saving Mode

```
light.setPowerMode(VEML7700_PSM_MODE2, true);
```

Power saving modes:

```
VEML7700_PSM_MODE1
VEML7700_PSM_MODE2
VEML7700_PSM_MODE3
VEML7700_PSM_MODE4
```

---

# Example Applications

Typical applications include:

* Automatic display brightness control
* Smart lighting systems
* Environmental light monitoring
* Industrial light sensing
* Outdoor light measurement
* Energy efficient lighting control

---

# License

MIT License

---

# Author

7Semi
