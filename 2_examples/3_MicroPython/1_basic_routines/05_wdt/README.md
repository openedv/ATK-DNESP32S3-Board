## watch dog example

### 1 Brief

The main function of this code is to watchdog function.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- LED - IO1

- XL9555

  - INT - IO0
  - SDA - IO41
  - CLK - IO42

The watchdog uses a timer peripheral, which is an internal peripheral,so there is no corresponding connection schematic diagram.

### 3 Running

#### 3.1 Download

If you need to download the code, please refer to the 3.3 Running Offline section in the [Developing With MicroPython tutorial](../../../../1_docs/Developing_With_MicroPython.md), which provides a detailed download process.

#### 3.2 Phenomenon

Press the reset button to restart the development board, and observe that the LED on the development board is constantly flashing, proving that the system is constantly reset. At this time, we try to press the KEY0 button repeatedly, and you can see that the LED will be on.

