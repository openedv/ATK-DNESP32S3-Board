## ds18b20 example

### 1 Brief

The main function of this code is to learn the temperature sensors.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- UART0

	- TXD0 - IO43
	- RXD0 - IO44
- XL9555
	- IIC_SCL - IO42
	- IIC_SDA - IO41
	- IIC_INT - IO0(jumper cap connection)
- SPI_LCD
	- CS - IO21
	- SCK - IO12
	- SDA - IO11
	- DC - IO40(jumper cap connection)
	- PWR - XL9555_P13
	- RST - XL9555_P12
- DQ - IO0(jumper cap connection)

<img src="../../../../1_docs/3_figures/examples/one_wire/ds18b20_sch.png" style="zoom:50%;" />

The place position of the ds18b20 in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/one_wire/ds18b20_position.png)

### 3 Running

#### 3.1 Compile & Download

**1 Compilation process**

- Connect the UART interface in the upper left corner of the image to the computer using a USB cable
- 17_ds18b20 routine opened using Arduino IDE2
- Select Target Board (① Arduino IDE: Tools --> Board --> esp32 --> ESP32S3 Dev Module)
- Select UART port number (② Arduino IDE: Tools --> port --> COMx)
- Set ESP32S3 parameter (③ Arduino IDE: Tools --> esp32s3 parameter(please see led example))
- Compile arduino project (④ Arduino IDE: "Sketch --> Verify/Compile" or "Ctrl+R" or "Verify button")
- Wait for compilation success (⑤ Arduino IDE: Output Window --> show "Done compiling")

**2 Download process**

- Download (⑥ Arduino IDE: "Sketch --> Upload" or "Ctrl+U" or "Upload button")

#### 3.2 Phenomenon

After normal operation, the LCD displays the temperature data collected by the DS18B20 temperature sensor.

![](../../../../1_docs/3_figures/examples/one_wire/ds18b20_phe.png)