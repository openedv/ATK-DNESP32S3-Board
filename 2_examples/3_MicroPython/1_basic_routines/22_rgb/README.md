## rgb example

### 1 Brief

The main function of this code is to learn the rgblcd.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- UART0

  - TXD0 - IO43
  - RXD0 - IO44
- XL9555
  - IIC_SCL - IO42
  - IIC_SDA - IO41
  - IIC_INT - IO0(jumper cap connection)
- RGB
  - LCD_BL- XL9555_P13
  - LCD_DE - IO4
  - LCD_VSYNC - NC
  - LCD_HSYNC - NC
  - LCD_PCLK - IO5
  - LCD_R3 - IO45
  - LCD_R4 - IO48
  - LCD_R5 - IO47
  - LCD_R6 - IO21
  - LCD_R7 - IO14
  - LCD_G2 - IO10
  - LCD_G3 - IO9
  - LCD_G4 - IO46
  - LCD_G5 - IO3
  - LCD_G6 - IO8
  - LCD_G7 - IO18
  - LCD_B3 - IO17
  - LCD_B4 - IO16
  - LCD_B5 - IO15
  - LCD_B6 - IO7
  - LCD_B7 - IO6

![](../../../../1_docs/3_figures/examples/rgb/rgb_sch.png)

The position of the qma6100p in the development board is shown as follows:

![](../../../../1_docs/3_figures/examples/rgb/RGB_position.png)

### 3 Running

#### 3.1 Download

If you need to download the code, please refer to the 3.3 Running Offline section in the [Developing With MicroPython tutorial](../../../../1_docs/Developing_With_MicroPython.md), which provides a detailed download process.

#### 3.2 Phenomenon

RGBLCD screen refreshes with different colors.

![](../../../../1_docs/3_figures/examples/sdcard/spilcd_phenomenon_mpy.png)