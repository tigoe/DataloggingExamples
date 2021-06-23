# Introduction to the Feather M0 Adalogger

In this workshop you'll be working with the Adafruit Feather M0 Adalogger microcontroller board. This board uses a Cortex ARM M0+, which is the same processor as the Arduino Zero. That means it's Arduino-compatible, and a very capable processor for datalogging.  The [Adafruit Guide to the Feather M0 Adalogger](https://learn.adafruit.com/adafruit-feather-m0-adalogger?view=all) is a thorough introduction. This page is a quick-reference only.

## Functions of the Board

  The Feather M0 Adalogger can read various sensors using its digital and analog inputs and its asynchronous serial and I2C communications pins. It can also read and write from an attached SD card. It can communicate with a personal computer via USB as well. It has a Lithium-Poly (LiPo) battery charger built-in so you can charge a battery while you're programming, then unplug and run off the battery independently. There's also a built-in real-time clock that can keep accurate time and two built-in LEDs that you can use to indicate what's going on.

  Below is the pin diagram for the board. Many of the pins have multiple functions. For the purposes of this workshop, the important pins are as follows:

  * **(Digital I/O pins (D4-D13))**  You can attach digital inputs (switches and pushbuttons) and digital outputs (LEDs, etc) to these pins to control them using the digitalRead() and digitalWrite() commands
  * **(Analog input pins (A0-A5))**  You can attach sensors that produce a changing analog voltage to these pins and read them using the analogRead() command. Potentiometers, light sensors, distance sensors, and force sensors are good examples of this kind of sensor.
  * **(SPI pins (MISO, MOSI, SCK))**  The pins labeled MISO, MOSI, and SCK are for synchronous serial communication using the SPI protocol. They control the SD card. You shouldn't use them for other inputs or outputs
  * **(I2C pins (SDA, SCL))**  The pins labeled SDA and SCL are for synchronous serial communications using the I2C protocol. These are used for communicating with sensors like the LIS3DH accelerometer and others.


[![Feather M0 Adalogger pin diagram" align="middle](images/Feather_M0_Adalogger.png)](images/Feather_M0_Adalogger.png)

_Adalogger M0 pin diagram. [Click here](images/adafruit_products_p2796.png) 
for a more detailed version. For more on the pin functions, see [this link.](https://learn.adafruit.com/adafruit-feather-m0-adalogger?view=all#pinouts)_

## Setting up the board

To test your board, click the Tools Menu --> Boards option, and choose "Adafruit Feather M0 (Native USB Port)". Then click the Tools menu --> Port option, and choose the port with "(Adafruit Feather M0)" at the end of it. THen click the File Menu --> Examples option --> Basics --> Blink to open a sketch that will blink the red LED on the board. Click the Upload button from the toolbar and you should see the sketch upload and the red LED will begin to blink.

[![The Arduino Toolbar" align="middle](images/toolbar.png)](images/toolbar.png)

_(The Arduino IDE toolbar)_

## Connecting the Sensors

Each of the examples in this repository uses a different sensor with the Feather, and you'll need to wire the sensor to the board before uploading the code that reads it. Disconnect your board from power before wiring your sensors. Below are the wiring diagrams for each sensor.

###  Pushbutton
[![Pushbutton connected to a Feather" align="middle](images/pushbutton_bb.png)](images/pushbutton_bb.png)
_(Pushbutton connected to a Feather. Button output pin is attached to digital pin 5 of the microcontroller.)_

###  Potentiometer
[![Potentiometer connected to a Feather" align="middle](images/potentiometer_bb.png)](images/potentiometer_bb.png)
_(Potentiometer connected to a Feather. Potentiometer output pin is attached to analog pin 0 of the microcontroller.)_ 


###  DHT11 Temperature and Humidity Sensor
[![DHT11 Temp/Humidity sensor connected to a Feather" align="middle](images/DHT11_bb.png)](images/DHT11_bb.png)
_(DHT11 Temp/Humidity sensor connected to a Feather. Sensor output pin is attached to digital pin 5 of the microcontroller.)_ 

[Code example](https://github.com/tigoe/DataloggingExamples/blob/main/Columbia_Brown_Center_2016/DHT11/DHT11.ino)

[Code example - Temp/Humidity Datalogger](https://github.com/tigoe/DataloggingExamples/blob/main/Columbia_Brown_Center_2016/TempHumidityLogger/TempHumidityLogger.ino)

###  Microphone
[![MAX4466 microphone connected to a Feather" align="middle](images/MAX4466_bb.png)](images/MAX4466_bb.png)
_(Max4466 Microphone connected to a Feather. Microphone output pin is attached to analog pin 0 of the microcontroller.)_ 

[Code example - Loudness](https://github.com/tigoe/DataloggingExamples/blob/main/Columbia_Brown_Center_2016/Loudness/Loudness.ino)

[Code example - Frequency Meter](https://github.com/tigoe/DataloggingExamples/blob/main/Columbia_Brown_Center_2016/FrequencyMeter/FrequencyMeter.ino)


###  Light Sensor
[![GA1A12S202 Log-scale light sensor connected to a Feather" align="middle](images/GA1A12S202_bb.png)](images/GA1A12S202_bb.png)
_(GA1A12S202 Log-scale light sensor connected to a Feather. Sensor output pin is attached to analog pin 0 of the microcontroller.)_ 

[Code example - Read Lux](https://github.com/tigoe/DataloggingExamples/blob/main/Columbia_Brown_Center_2016/GA1AS202LightSensor/GA1AS202LightSensor.ino)


### Accelerometer
[![LIS3DH accelerometer connected to a Feather" align="middle](images/LIS3DH_bb.png)](images/LIS3DH_bb.png)
_(LIS3DH accelerometer connected to a Feather. Sensor SDA and SCL pins are attached to SDA and SCL pins of the microcontroller.)_ 

[Code example - Read g's](https://github.com/tigoe/DataloggingExamples/blob/main/Columbia_Brown_Center_2016/LIS3DHAccelerometer/LIS3DHAccelerometer.ino)

### Ultrasonic Distance Ranger
[![Maxbotix LV-EZ0 Ultrasonic Distance Ranger connected to a Feather" align="middle](images/Maxbotix_bb.png)](images/Maxbotix_bb.png)
_(Maxbotix LV-EZ0 Ultrasonic Distance Ranger connected to a Feather. Sensor output pin is attached to analog pin 0 of the microcontroller.)_ 

### PIR Motion Sensor
[![PIR motiomn sensor connected to a Feather" align="middle](images/PIR_sensor_bb.png)](images/PIR_sensor_bb.png)
_(PIR motion sensor connected to a Feather. Sensor output pin is attached to digital pin 5 of the microcontroller.)_ 

[Code example](https://github.com/tigoe/DataloggingExamples/blob/main/Columbia_Brown_Center_2016/PIR_sensor/PIR_sensor.ino)

## All Code Examples
There are additional code examples for the se sensors, and for the SD card and real-time clock at [this link](https://github.com/tigoe/DataloggingExamples/tree/main/Columbia_Brown_Center_2016).