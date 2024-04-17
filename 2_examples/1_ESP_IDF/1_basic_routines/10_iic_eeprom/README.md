## iic eeprom example

### 1 Brief

The main function of this code is to read and write the EEPROM device.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- KEY - IO0
- UART0

  - TXD0 - IO43
  - RXD0 - IO44
- EEPROM
  - IIC_SCL - IO42
  - IIC_SDA - IO41

<img src="../../../../1_docs/3_figures/examples/iic/eeprom_sch.png" style="zoom: 50%;" />

The position of the EEPROM in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/iic/eeprom_position.png)

### 3 Running

#### 3.1 Compilation and Download

There are two ways to download code for ESP32S3.

##### 3.1.1 USB UART

![](../../../../1_docs/3_figures/examples/led/compilation(UART).png)

**1 Compilation process**

- Connect the USB UART on the DNESP32S3 development board to your computer using a USB data cable
- Open the '10_iic_eeprom' example using VS Code
- Select UART port number(Figure ①:ESP-IDF: Select Port to Use (COM, tty, usbserial))
- Set Engineering Target Chip(Figure ②:ESP-IDF: Set Espressif Device Target)
- Clearing project engineering(Figure ③:ESP IDF: Full Clean)
- Select Flash Method(Figure ⑤:ESP-IDF: Select Flash Method)
- Build Project(Figure ④:ESP-IDF: Build Project)

**2 Download process**

- Download(Figure ⑥:ESP-IDF: Flash Device)

##### 3.1.2 JTAG(USB)

![](../../../../1_docs/3_figures/examples/led/compilation(JTAG).png)

**1 Compilation process**

- Connect the USB(JTAG) on the DNESP32S3 development board to your computer using a USB data cable
- Open the '10_iic_eeprom' example using VS Code
- Select JTAG port number(Figure ①:ESP-IDF: Select Port to Use (COM, tty, usbserial))
- Clearing project engineering(Figure ③:ESP IDF: Full Clean)
- Select Flash Method(Figure ⑤:ESP-IDF: Select Flash Method)
- Build Project(Figure ④:ESP-IDF: Build Project)

**2 Download process**

- Download(Figure ⑥:ESP-IDF: Flash Device)

#### 3.2 Phenomenon

After normal operation, Control the data writing of 24C02 through the KEY0 button, and control the data reading of 24C02 through the KEY1 button.

