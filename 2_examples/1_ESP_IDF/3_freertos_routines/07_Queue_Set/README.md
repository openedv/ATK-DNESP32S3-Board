## Queue_Set example

### 1 Brief

The main function of this code is to learn queue set.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- LED - IO0
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

### 3 Running

#### 3.1 Compile & Download

There are two ways to download code for ESP32S3.

##### 3.1.1 USB UART

![](../../../../../../../../1，研发一部(MCU)/8，最新版本光盘目录结构/ESP32S3海外上市项目(新)/dnesp32s3-board/1_docs/3_figures/examples/led/compilation(UART).png)

**1 Compilation process**

- Connect the USB UART to the computer using a USB cable
- Using VS Code to open the 07_Queue_Set example
- Select UART port number(Figure ①:ESP-IDF: Select Port to Use (COM, tty, usbserial))
- Set Engineering Target Chip(Figure ②:ESP-IDF: Set Espressif Device Target)
- Clearing project engineering(Figure ③:ESP IDF: Full Clean)
- Select Flash Method(Figure ⑤:ESP-IDF: Select Flash Method)
- Build Project(Figure ④:ESP-IDF: Build Project)

**2 Download process**

- Download(Figure ⑥:ESP-IDF: Flash Device)

##### 3.1.2 JTAG(USB)

![](../../../../../../../../1，研发一部(MCU)/8，最新版本光盘目录结构/ESP32S3海外上市项目(新)/dnesp32s3-board/1_docs/3_figures/examples/led/compilation(JTAG).png)

**1 Compilation process**

- Connect the TATG(USB) to the computer using a USB cable
- Using VS Code to open the 07_Queue_Set example
- Select JTAG port number(Figure ①:ESP-IDF: Select Port to Use (COM, tty, usbserial))
- Clearing project engineering(Figure ③:ESP IDF: Full Clean)
- Select Flash Method(Figure ⑤:ESP-IDF: Select Flash Method)
- Build Project(Figure ④:ESP-IDF: Build Project)

**2 Download process**

- Download(Figure ⑥:ESP-IDF: Flash Device)

#### 3.2 Phenomenon

1. The spilcd module will display experimental message.
2. When the key is pressed, a message is correspondingly sent to the queue and a binary semaphore is released.
3. Task2 obtains members from the queue set, and according to the different members, receives messages from the queue with a binary semaphore, and sends the results display serial port.
4. The LED blinks, indicating that the system is running.

![]()