## exti example

### 1 Brief

The main function of this code is to make the onboard BOOT key control the status of the red LED by an external interrupt.

### 2 Hardware Hookup

The hardware resources used in this experiment are:

- LED - IO1
- KEY - IO0

The hardware is consistent with the **key example**, and will not be described in detail here.

### 3 Running

#### 3.1 Download

If you need to download the code, please refer to the 3.3 Running Offline section in the [Developing With MicroPython tutorial](../../../../1_docs/Developing_With_MicroPython.md), which provides a detailed download process.

#### 3.2 Phenomenon

After normal operation, press the BOOT key to trigger an external interrupt, and the LED indicator status changes from on to off, or from off to on.

![]()

