/**
 ****************************************************************************************************
 * @file        xl9555.h
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

#ifndef __XL9555_H
#define __XL9555_H

#include "Arduino.h"

/* pin definition */
#define IIC_SCL       42
#define IIC_SDA       41
#define IIC_INT_PIN   0     /* a jumper cap is required for connection */

#define EXIO_ADDR     0x20  /* xl9555 device address(7bit) */

#define IIC_INT       digitalRead(IIC_INT_PIN) 

/* xl9555 pin function */
#define KEY0                          XL_PIN_P17  /* KEY0_Pin P17 */    
#define KEY1                          XL_PIN_P16  /* KEY1_Pin P16 */    
#define KEY2                          XL_PIN_P15  /* KEY2_Pin P15 */    
#define KEY3                          XL_PIN_P14  /* KEY3_Pin P14 */    
#define SLCD_PWR                      XL_PIN_P13  /* SPI_LCD BL_Pin P13 */    
#define SLCD_RST                      XL_PIN_P12  /* SPI_LCD RST_Pin P12 */    
#define CT_RST                        XL_PIN_P11  /* TP INT_Pin P11 */    
#define LCD_BL                        XL_PIN_P10  /* RGBLCD BL_Pin P10 */    
#define GBC_KEY                       XL_PIN_P07  /* ATK_MODULE KEY_Pin P07 */
#define GBC_LED                       XL_PIN_P06  /* ATK_MODULE LED_Pin P06 */
#define OV_RESET                      XL_PIN_P05  /* CAMERA RST_Pin P05 */
#define OV_PWDN                       XL_PIN_P04  /* CAMERA PWDN_Pin P04 */
#define BEEP                          XL_PIN_P03  /* BEEP Ctl_Pin P03 */
#define SPK_EN                        XL_PIN_P02  /* SPK EN_Pin P02 */
#define QMA_INT                       XL_PIN_P01  /* QMA6100P INT_Pin P01 */
#define AP_INT                        XL_PIN_P00  /* AP3216C INT_Pin P00 */

/* device register */
#define XL9555_INPUT_PORT0_REG        0           /* Input port 0 register */
#define XL9555_INPUT_PORT1_REG        1           /* Input port 1 register */
#define XL9555_OUTPUT_PORT0_REG       2           /* Output port 0 register */
#define XL9555_OUTPUT_PORT1_REG       3           /* Output port 1 register */
#define XL9555_INVERSION_PORT0_REG    4           /* Polarity Inversion port 0 register */
#define XL9555_INVERSION_PORT1_REG    5           /* Polarity Inversion port 1 register */
#define XL9555_CONFIG_PORT0_REG       6           /* Configuration port 0 register */
#define XL9555_CONFIG_PORT1_REG       7           /* Configuration port 1 register */

/* xl9555 pin value */
#define XL_PIN_P00       				      0x0001
#define XL_PIN_P01       				      0x0002
#define XL_PIN_P02       				      0x0004
#define XL_PIN_P03       				      0x0008
#define XL_PIN_P04       				      0x0010
#define XL_PIN_P05       				      0x0020
#define XL_PIN_P06       				      0x0040
#define XL_PIN_P07       				      0x0080
#define XL_PIN_P10      				      0x0100
#define XL_PIN_P11      				      0x0200
#define XL_PIN_P12      				      0x0400
#define XL_PIN_P13      				      0x0800
#define XL_PIN_P14      				      0x1000
#define XL_PIN_P15      				      0x2000
#define XL_PIN_P16      				      0x4000
#define XL_PIN_P17      				      0x8000

#define XL_PORT0_ALL_PIN              0x00FF
#define XL_PORT1_ALL_PIN              0xFF00

/* pin mode */
typedef enum
{
  IO_SET_OUTPUT = 0x00,  
  IO_SET_INPUT,       
} io_mode_t;

/* pin state */
typedef enum
{
  IO_SET_LOW = 0x00,  
  IO_SET_HIGH,       
} io_state_t;

/* function declaration */
void xl9555_init(void);                                     
void xl9555_write_reg(uint8_t reg, uint8_t data);          
uint8_t xl9555_read_reg(uint8_t reg);                       
void xl9555_write_port(uint8_t portx, uint8_t data);        
uint8_t xl9555_read_port(uint8_t portx);                   
void xl9555_io_config(uint16_t port_pin, io_mode_t mode);   
void xl9555_pin_set(uint16_t port_pin, io_state_t state);   
uint8_t xl9555_get_pin(uint16_t port_pin);                  

#endif
