## exti example

### 1 Brief

The main function of this code is to make the onboard BOOT key control the status of the red LED by an external interrupt.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- LED - IO1
- KEY - IO0

The hardware is consistent with the **key example**, and will not be described in detail here.

### 3 Running

#### 3.1 Compile & Download

**1 Compilation process**

- Connect the UART interface in the upper left corner of the image to the computer using a USB cable
- 03_exti routine opened using Arduino IDE2
- Select Target Board (① Arduino IDE: Tools --> Board --> esp32 --> ESP32S3 Dev Module)
- Select UART port number (② Arduino IDE: Tools --> port --> COMx)
- Set ESP32S3 parameter (③ Arduino IDE: Tools --> esp32s3 parameter(please see led example))
- Compile arduino project (④ Arduino IDE: "Sketch --> Verify/Compile" or "Ctrl+R" or "Verify button")
- Wait for compilation success (⑤ Arduino IDE: Output Window --> show "Done compiling")

**2 Download process**

- Download (⑥ Arduino IDE: "Sketch --> Upload" or "Ctrl+U" or "Upload button")

#### 3.2 Phenomenon

After normal operation, press the BOOT key to trigger an external interrupt, and the LED indicator status changes from on to off, or from off to on.

![]()