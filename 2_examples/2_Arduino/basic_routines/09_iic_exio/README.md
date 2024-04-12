## iic_exio example

### 1 Brief

press KEY0 and KEY1 on the development board to control buzzer work, press KEY2 and KEY3 to control LED light off.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- LED - IO1
- UART0

	- TXD0 - IO43
	- RXD0 - IO44
- XL9555
	- IIC_SCL - IO42
	- IIC_SDA - IO41
	- IIC_INT - IO0(jumper cap connection)


<img src="../../../../1_docs/3_figures/examples/iic/xl9555_sch.png" style="zoom: 80%;" />

The position of the XL9555 in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/iic/xl9555_position.png)

### 3 Running

#### 3.1 Compile & Download

**1 Compilation process**

- Connect the UART interface in the upper left corner of the image to the computer using a USB cable
- 09_iic_exio routine opened using Arduino IDE2
- Select Target Board (① Arduino IDE: Tools --> Board --> esp32 --> ESP32S3 Dev Module)
- Select UART port number (② Arduino IDE: Tools --> port --> COMx)
- Set ESP32S3 parameter (③ Arduino IDE: Tools --> esp32s3 parameter(please see led example))
- Compile arduino project (④ Arduino IDE: "Sketch --> Verify/Compile" or "Ctrl+R" or "Verify button")
- Wait for compilation success (⑤ Arduino IDE: Output Window --> show "Done compiling")

**2 Download process**

- Download (⑥ Arduino IDE: "Sketch --> Upload" or "Ctrl+U" or "Upload button")

#### 3.2 Phenomenon

After normal operation, press KEY0 and KEY1 on the development board to control buzzer work, press KEY2 and KEY3 to control LED light off.

![]()