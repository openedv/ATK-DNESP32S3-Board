## infrared_transmission example

### 1 Brief

The main function of this code is to learn infrared transmission.

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
- REMOTE_IN - IO2
- REMOTE_OUT - IO8(jumper cap connection)

<img src="../../../../1_docs/3_figures/examples/remote/infrared_transmission_sch.png" style="zoom:50%;" />

The position of the infrared transmitting tube in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/remote/infrared_transmission_position.png)

### 3 Running

#### 3.1 Download

If you need to download the code, please refer to the 3.3 Running Offline section in the [Developing With MicroPython tutorial](../../../../1_docs/Developing_With_MicroPython.md), which provides a detailed download process.

#### 3.2 Phenomenon

After normal operation, according to the program Settings to send infrared remote control data, infrared receiving head received data, the LCD will display the key value and identification.

![](../../../../1_docs/3_figures/examples/remote/spilcd_phenomenon_mpy_tx.png)