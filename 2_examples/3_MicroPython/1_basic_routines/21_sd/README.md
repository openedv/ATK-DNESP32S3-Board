## sdcard example

### 1 Brief

The main function of this code is to learn the TF card module.

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
- SPI_LCD
  - CS - IO21
  - SCK - IO12
  - SDA - IO11
  - DC - IO40(jumper cap connection)
  - PWR - XL9555_P13
  - RST - XL9555_P12
- SD
  - SDCS - IO2
  - SCK - IO12
  - MOSI - IO11
  - MISO - IO13

<img src="../../../../1_docs/3_figures/examples/sdcard/sdcard_sch.png" style="zoom:50%;" />

The position of the sdcard in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/sdcard/sdcard_position.png)

### 3 Running

#### 3.1 Download

If you need to download the code, please refer to the 3.3 Running Offline section in the [Developing With MicroPython tutorial](../../../../1_docs/Developing_With_MicroPython.md), which provides a detailed download process.

#### 3.2 Phenomenon

After normal operation, the LCD displays the information (capacity) of the SD card. You can test the SD card by pressing a button and view it through the serial port assistant.

![]()