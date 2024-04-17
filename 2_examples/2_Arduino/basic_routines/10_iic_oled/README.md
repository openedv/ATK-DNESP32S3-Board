## iic_oled example

Note1: ALIENTEK's OLED Module module supports four types of interfaces, which can be configured via the BS1 and BS2 settings on the back of the module.The default setting for ALIENTEK's OLED Module is: BS1 and BS2 connected to VCC, which means it's using the 8080 parallel interface mode. 

For this experiment, the interface mode of the OLED module needs to be changed to I2C. Therefore, you should connect BS2 to GND and connect D1 and D2 together.

![](../../../../1_docs/3_figures/examples/oled/oled_iic_interface.png)

Note2: iic_oled example need to install "ESP8266 and ESP32 OLED driver for SSD1306 displays" LIBRARY.

![](../../../../1_docs/3_figures/examples/oled/oled_lib.png)

### 1 Brief

The main function of this code is to use a oled module to display experimental information.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- UART0

	- TXD0 - IO43
	- RXD0 - IO44
- XL9555
	- IIC_SCL - IO42
	- IIC_SDA - IO41
	- IIC_INT - IO0(jumper cap connection)
- OLED
	- D0(SCL) - IO40
	- D1(SDA) - IO41
	- D2 - IO6
	- DC - IO38

![](../../../../1_docs/3_figures/examples/oled/oled_sch.png)

The place position of the OLED module in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/oled/oled_position.png)

### 3 Running

#### 3.1 Compile & Download

**1 Compilation process**

- Connect the UART interface in the upper left corner of the image to the computer using a USB cable
- 10_iic_oled routine opened using Arduino IDE2
- Select Target Board (① Arduino IDE: Tools --> Board --> esp32 --> ESP32S3 Dev Module)
- Select UART port number (② Arduino IDE: Tools --> port --> COMx)
- Set ESP32S3 parameter (③ Arduino IDE: Tools --> esp32s3 parameter(please see led example))
- Compile arduino project (④ Arduino IDE: "Sketch --> Verify/Compile" or "Ctrl+R" or "Verify button")
- Wait for compilation success (⑤ Arduino IDE: Output Window --> show "Done compiling")

**2 Download process**

- Download (⑥ Arduino IDE: "Sketch --> Upload" or "Ctrl+U" or "Upload button")

#### 3.2 Phenomenon

After normal operation, the oled module will display experimental demo.

<img src="../../../../1_docs/3_figures/examples/oled/oled_phe.png" style="zoom:33%;" />