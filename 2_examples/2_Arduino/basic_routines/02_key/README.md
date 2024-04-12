## key example

### 1 Brief

The main function of this code is to control the LED through the onboard KEY BOOT key.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- LED - IO1
- KEY - IO0

<img src="../../../../1_docs/3_figures/examples/key/key_sch.png" style="zoom:50%;" />

The position of the KEY in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/key/key_position.png)

### 3 Running

#### 3.1 Compile & Download

**1 Compilation process**

- Connect the UART interface in the upper left corner of the image to the computer using a USB cable
- 02_key routine opened using Arduino IDE2
- Select Target Board (① Arduino IDE: Tools --> Board --> esp32 --> ESP32S3 Dev Module)
- Select UART port number (② Arduino IDE: Tools --> port --> COMx)
- Set ESP32S3 parameter (③ Arduino IDE: Tools --> esp32s3 parameter(please see led example))
- Compile arduino project (④ Arduino IDE: "Sketch --> Verify/Compile" or "Ctrl+R" or "Verify button")
- Wait for compilation success (⑤ Arduino IDE: Output Window --> show "Done compiling")

**2 Download process**

- Download (⑥ Arduino IDE: "Sketch --> Upload" or "Ctrl+U" or "Upload button")

#### 3.2 Phenomenon

After normal operation, press the BOOT key to turn on the red LED.Otherwise, the red LED is off.

![]()