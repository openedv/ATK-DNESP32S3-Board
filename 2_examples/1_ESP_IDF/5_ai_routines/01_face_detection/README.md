## face detection example

### 1 Brief

The main function of this code is to detect faces.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- LCD - ALIENTEK 1.3 or 2.4 inch screen
  - CS - IO21
  - SCK - IO12
  - SDA - IO11
  - DC - IO40(jumper cap connection)
  - PWR - XL9555_P13
  - RST - XL9555_P12


- XL9555
  - IIC_SCL - IO42
  - IIC_SDA - IO41
  - IIC_INT - IO0(jumper cap connection)

- CAMERA
  - OV_D0 - IO4
  - OV_D1 - IO5
  - OV_D2 - IO6
  - OV_D3 - IO7
  - OV_D4 - IO15
  - OV_D5 - IO16
  - OV_D6 - IO17
  - OV_D7 - IO18
  - OV_VSYNC - IO47
  - OV_HREF - IO48
  - OV_PCLK - IO45
  - OV_SCL - IO38
  - OV_SDA - IO39
  - OV_PWDN - XL9555_P4
  - OV_RESET - XL9555_P5

The position of the CAMERA in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/face_detection_project/camera_sch.png)

### 3 Running

#### 3.1 Compile & Download

There are two ways to download code for ESP32S3.

##### 3.1.1 USB UART

![](../../../../1_docs/3_figures/examples/led/compilation(UART).png)

**1 Compilation process**

- Connect the USB UART to the computer using a USB cable
- Using VS Code to open the 01_face_detection example
- Select UART port number (Figure ①:ESP-IDF: Select Port to Use (COM, tty, usbserial))
- Set Engineering Target Chip (Figure ②:ESP-IDF: Set Espressif Device Target)
- Clearing project engineering（Figure ③:ESP IDF: Full Clean）
- Select Flash Method (Figure ⑤:ESP-IDF: Select Flash Method)
- Build Project (Figure ④:ESP-IDF: Build Project)

**2 Download process**

- Download（Figure ⑥:ESP-IDF: Flash Device）

##### 3.1.2 JTAG(USB)

![](../../../../1_docs/3_figures/examples/led/compilation(JTAG).png)

**1 Compilation process**

- Connect the TATG(USB) to the computer using a USB cable
- Using VS Code to open the 01_face_detection example
- Select JTAG port number (Figure ①:ESP-IDF: Select Port to Use (COM, tty, usbserial))
- Clearing project engineering（Figure ③:ESP IDF: Full Clean）
- Select Flash Method (Figure ⑤:ESP-IDF: Select Flash Method)
- Build Project (Figure ④:ESP-IDF: Build Project)

**2 Download process**

- Download（Figure ⑥:ESP-IDF: Flash Device）

#### 3.2 Phenomenon

Perform face detection on images.

