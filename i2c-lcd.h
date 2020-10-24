#ifndef _I2C_LCD_H
#define _I2C_LCD_H
/**
 * LCD with I2C wing attached at address 0x4E (write) 0x4F (read) 
 *
 * Jan-Willem Smaal <usenet@gispen.org> 5 June 2020 for ATMEL AVR 
 * Ported from AVR C to C++ on 24 Oct 2020 for ARM MBED also 
 * by Jan-Willem Smaal.   
 *******************************************************************
 */
#include <inttypes.h>
#include "mbed.h"

/*
 * I2C IO expander wing specifics  
 */ 
#define PCF8574T_WRITE 0x4E
#define PCF8574T_READ  0x4F 

#define RS (1<<0)
#define RW (1<<1)
#define EN (1<<2)
#define BT (1<<3)
#define DB4 (1<<4)
#define DB5 (1<<5)
#define DB6 (1<<6)
#define DB7 (1<<7)


/* 
 * Hitachi LCD datasheet defines 
 */ 
// We define DB0-4 but it's not used in 4 bit mode on the interface
// this just makes reading the datasheet easier. 
#define DB0 (1<<0)
#define DB1 (1<<1)
#define DB2 (1<<2)
#define DB3 (1<<3)

// Page 28 of HD44780U datasheet 
#define CLEAR_DISPLAY		DB0  
#define RETURN_HOME			DB1
#define ENTRY_MODE_SET		DB2 
#define DISPLAY_ON			DB3 
#define CURSOR_SHIFT		DB4 
#define FUNCTION_SET		DB5 
#define SET_CGRAM_ADDRESS	DB6 


// Shift cursor to RIGHT I/D == 0 or LEFT I/D == 1 
#define ENTRY_MODE_SHIFT_LEFT		(ENTRY_MODE_SET | (~DB1))
#define ENTRY_MODE_SHIFT_RIGHT		(ENTRY_MODE_SET |  (DB1)) 

// Shift display if S =1 to RIGHT I/D == 0 or LEFT I/D == 1 
#define ENTRY_MODE_DISPLAY_SHIFT_LEFT	(DB0 | ENTRY_MODE_SHIFT_LEFT) 
#define ENTRY_MODE_DISPLAY_SHIFT_RIGHT	(DB0 | ENTRY_MODE_SHIFT_RIGHT) 



class I2cLcd {
public: 
	/**
	 * Moves the cursor 40 positions to the right 
	 * so it ends up on line2. 
	 */ 
	void move_cursor_line2(void); 

	/**
	 * Puts the cursor back at line2
	 */
	void move_cursor_line1(void); 

	/**
	 * Takes care of sending the I2C start stop to the correct
	 * IO expander.  
	 */ 
	void ioexpander_write(uint8_t value);

	/**
	 * Takes care of sending the 8 bit byte
	 * in 2 4 bit nibbles.   
	 */
	void write4bits(uint8_t val);

	/**
	 * Simply write to the LCD
	 */
	void write(uint8_t val);

	/**
	 * Place a character on the LCD
	 */ 
	void putchar(char c); 

	/**
	 * Toggle enable pin
	 */
	void pulse_enable(uint8_t val);

   /**
	* We init the LCD in 4 bit mode. 
	*/ 
	void init();
};


#endif	// __I2C_LCD_H 
/* EOF */
