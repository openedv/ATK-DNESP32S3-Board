/**
 ****************************************************************************************************
 * @file        xl9555.cpp
 * @author      ALIENTEK
 * @brief       XL9555 code
 * @license     Copyright (C) 2020-2030, ALIENTEK
 ****************************************************************************************************
 * @attention
 *
 * platform     : ALIENTEK DNESP32S3 board
 * website      : www.alientek.com
 * forum        : www.openedv.com/forum.php
 *
 * change logs  :
 * version      data         notes
 * V1.0         20240430     the first version
 *
 ****************************************************************************************************
 */

#include "xl9555.h"
#include <Wire.h>

/**
* @brief       configure xl9555
* @param       None
* @retval      None
*/
void xl9555_init(void)
{
    pinMode(IIC_INT_PIN, INPUT_PULLUP);     /* set XL9555 interrupt pin as INPUT_PULLUP */

    Wire.begin(IIC_SDA, IIC_SCL, 400000);   /* iic initialization */

    /* read Pin State to clear interrupt signal */
    xl9555_read_port(0);
    xl9555_read_port(1);
}

/**
 * @brief      write data to xl9555 register
 * @param      reg    : xl9555 register address
 * @param      data   : write to xl9555 register data
 * @retval     None
 */
void xl9555_write_reg(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(EXIO_ADDR);        /* i2c address of device */
    Wire.write(reg);                          /* register to write */
    Wire.write(data);                         /* data to write */
    Wire.endTransmission();                   /* end */          
}

/**
 * @brief       read data from xl9555 register
 * @param       reg    : xl9555 register address
 * @retval      register data / 0xFF:no data
 */
uint8_t xl9555_read_reg(uint8_t reg)
{
    Wire.beginTransmission(EXIO_ADDR);        /* i2c address of device */
    Wire.write(reg);                          /* register to read */
    Wire.endTransmission(0);                  /* end */

    Wire.requestFrom(EXIO_ADDR, 1);           /* number of bytes to read */
    if (Wire.available() != 0)                /* have data? */
    {
        return Wire.read();                   /* read data */
    }

    return 0xFF;
}

/**
 * @brief       set xl9555 Port0 or Port1 output level
 * @param       portx : P0 / P1
 * @param       data  : IO_State(corresponds to eight pins)
 * @retval      None
 */
void xl9555_write_port(uint8_t portx, uint8_t data)
{
    xl9555_write_reg(portx ? XL9555_OUTPUT_PORT1_REG : XL9555_OUTPUT_PORT0_REG, data);
}

/**
 * @brief       read XL9555 Port0 or Port1 input level
 * @param       portx : P0 / P1
 * @retval      data  : IO_State(corresponds to eight pins)
 */
uint8_t xl9555_read_port(uint8_t portx)
{
    return xl9555_read_reg(portx ? XL9555_INPUT_PORT1_REG : XL9555_INPUT_PORT0_REG);
}

/**
 * @brief       set xl9555 pin as output or input 
 * @param       port_pin  : pin number,P0~7 or P1~7
 * @param       mode      : IO_SET_OUTPUT / IO_SET_INPUT
 * @retval      None
 */
void xl9555_io_config(uint16_t port_pin, io_mode_t mode)
{
    uint8_t config_reg = 0;
    uint8_t config_value = 0;

    config_reg  = xl9555_read_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_CONFIG_PORT1_REG : XL9555_CONFIG_PORT0_REG);   /* read the register in which the Pin is set */

    if (mode == IO_SET_OUTPUT)    /* the input and output settings are set according to the mode parameter, which does not affect other IO */
    {
        /* gets the port value of an IO that is set to the output function but does not affect the status of other IO that is not set */
        config_value = config_reg & (~(port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0)));   
    }
    else
    {
        /* gets the port value of an IO that is set to the input function without affecting the status of other IO that is not set */
        config_value = config_reg | (port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0));      
    }

    xl9555_write_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_CONFIG_PORT1_REG : XL9555_CONFIG_PORT0_REG, config_value);    /* Set IO input/output status to the configuration register */
}

/**
 * @brief       set xl9555 output pin level 
 * @param       port_pin  : outpin pin number
 * @param       state     : IO_SET_LOW / IO_SET_HIGH
 * @retval      None
 */
void xl9555_pin_set(uint16_t port_pin, io_state_t state)
{
    uint8_t pin_reg = 0;
    uint8_t pin_value = 0;

    pin_reg = xl9555_read_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_OUTPUT_PORT1_REG : XL9555_OUTPUT_PORT0_REG);     /* read the register in which the Pin is set */

    if (state == IO_SET_HIGH)    /* set IO high and low levels according to the state parameter */
    {
        pin_value = pin_reg | (port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0));          /* gets the port value of an IO set to high but does not affect the status of other IO not set */
    }
    else
    {
        pin_value = pin_reg & (~(port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0)));       /* gets the port value of an IO set to low but does not affect the status of other IO not set */
    }

    xl9555_write_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_OUTPUT_PORT1_REG : XL9555_OUTPUT_PORT0_REG, pin_value);   /* sets the IO high-low level status to the output register */
}

/**
 * @brief       get xl9555 input pin level 
 * @param       port_pin  : input pin number
 * @retval      state     : 1 / 0
 */
uint8_t xl9555_get_pin(uint16_t port_pin)
{
    uint8_t pin_state = 0;
    uint8_t port_value = 0;

    port_value = xl9555_read_reg(port_pin > XL_PORT0_ALL_PIN ? XL9555_INPUT_PORT1_REG : XL9555_INPUT_PORT0_REG);  /* read the status of the port where the pin resides: 1 is not pressed, 0 is pressed */
    pin_state = port_pin >> (port_pin > XL_PORT0_ALL_PIN ? 8 : 0);    /* if the pin is PORT1, move 8 bits to the right */
    pin_state = pin_state & port_value;                               /* get the status of the bit to be queried */

    return pin_state ? 1 : 0;
}
