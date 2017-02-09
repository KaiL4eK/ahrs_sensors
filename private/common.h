#ifndef COMMON_H_
#define	COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

typedef void * i2c_module_t;
typedef void * uart_module_t;

extern void 	UART_write_string( uart_module_t module, const char *string, ... );

extern uint8_t 	i2c_read_byte 	( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr );
extern void 	i2c_read_bytes 	( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr, uint8_t lenght, uint8_t *data);
extern void 	i2c_write_bytes ( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr, uint8_t lenght, uint8_t *data );
extern void 	i2c_write_byte 	( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr, uint8_t data );
extern void 	i2c_write_word 	( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr, uint16_t data );

extern uint8_t 	i2c_read_bit 	( i2c_module_t p_module, uint8_t slave_addr, uint8_t eeprom_addr, uint8_t bit_start );
extern uint8_t 	i2c_read_bits 	( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr, uint8_t bit_start, uint8_t length );
extern void 	i2c_write_bit 	( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr, uint8_t bit_start, uint8_t data );
extern void 	i2c_write_bits  ( i2c_module_t p_module, uint8_t i2c_address, uint8_t reg_addr, uint8_t bit_start, uint8_t length, uint8_t data );

#endif	/* COMMON_H_ */

