/**
 * LCD with I2C wing attached at address 0x4E (write) 0x4F (read) 
 *
 * Jan-Willem Smaal <usenet@gispen.org> 5 June 2020
 *******************************************************************
 */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "util.h" 


/** 
 * I2C_master modified from original of Peter Fleury 
 */
#include "I2C_master.h"


/**
 * I2C LCD interface using io expander (PCF8574F) wing FC-113. 
 * by Jan-Willem Smaal <usenet@gispen.org> 
 */  
#include "i2c-lcd.h"


/**
 * Moves the cursor 40 positions to the right
 * so it ends up on line2.
 */
void i2c_lcd_move_cursor_line2(void){
	uint8_t j; 

	i2c_lcd_write(RETURN_HOME);
	for(j=0;j<40;j++) {
		i2c_lcd_write(0b00010100);
	}

	return;
} 
	

/**
 * Puts the cursor back at line2 
 */ 
void i2c_lcd_move_cursor_line1(void){
	i2c_lcd_write(RETURN_HOME);

	return;
} 
	


/**
 * Takes care of sending the I2C start stop to the correct
 * IO expander.  
 */ 
void i2c_ioexpander_write(uint8_t value) 
{
	i2c_start(PCF8574T_WRITE);
	i2c_write(value);
	i2c_stop();

	return;
}


/**
 * Takes care of sending the 8 bit byte 
 * with the enable pin pulsed (required for 4 bit mode)     
 */
void i2c_lcd_write4bits(uint8_t val) 
{
    i2c_ioexpander_write(val);	
	i2c_lcd_pulse_enable(val);

	return;
}


/**
 * in 4 bits mode we need to pulse the bytes in.  
 * one nibble at a time. 
 */
void i2c_lcd_write(uint8_t val) 
{
	uint8_t lnibble, hnibble; 
	hnibble = val & 0xF0;	// High nibble 4 bits
	lnibble = val & 0x0F; // Low  nibble 4 bits 
	i2c_lcd_write4bits(hnibble);	
	i2c_lcd_write4bits(lnibble<<4);	
	
	return;
}



/**
 * Output a character in 4 bit mode. 
 */ 
void i2c_lcd_putchar(char c) 
{
	//i2c_lcd_write((uint8_t) c);


	uint8_t lnibble, hnibble; 
	hnibble = c & 0xF0;	// High nibble 4 bits
	lnibble = c & 0x0F; // Low  nibble 4 bits 
	i2c_lcd_write4bits(RS | BT | hnibble);	
	i2c_lcd_write4bits(RS | BT | lnibble<<4);	

	return;
}


/**
 * Toggle enable pin
 */
void i2c_lcd_pulse_enable(uint8_t val)
{
    i2c_ioexpander_write((val|BT) | EN);		// Enable  high
    _delay_ms(1);						// >450ns

	i2c_ioexpander_write((val|BT) & ~EN);	// Enable low
    _delay_ms(1);						// > 37us e

	return; 
}


/**
 * We init the LCD in 4 bit mode. 
 *
 * Just following the Hitachi datasheet. 
 */ 
void i2c_lcd_init()
{
	uint8_t i, j, lnibble, hnibble; 

	// We really need to wait a long time 
	// to ensure the LCD is ready after powerup. 
	_delay_ms(600);
	i2c_ioexpander_write(~BT);
	
#if 0  
	// !! 
	i2c_lcd_write4bits(DB6| DB5);
	_delay_ms(5); 
	// !! 
	i2c_lcd_write4bits(DB6| DB5);
	_delay_ms(5); 
	// !! 
	i2c_lcd_write4bits(DB6| DB5);
	_delay_ms(5); 
#endif 


	// page42 Step 2 -> 4 bit mode. 
	i2c_lcd_write4bits(DB5);
	_delay_ms(5); 

	// page42 Step 3a
	i2c_lcd_write4bits(DB5);	// One line. 
	_delay_ms(5); 


	// page42 Step 3b: set 4 bit operation and select 1 line displays
	// DB5 == function set 
	i2c_lcd_write4bits(DB5);
	_delay_ms(150);


	/* 
	 * !! From this point on 4 bit operation is used !! 
	 */ 
	//i2c_lcd_write(0b00101100);
	i2c_lcd_write(DB5 | DB3 | DB2);
	_delay_ms(150); 



	// page42 Step 4: Turn on display and cursor so 0000 1111 
	// which means DB0,1,2,3 actually.  
	i2c_lcd_write4bits(0x00);
	//i2c_lcd_write4bits(DB7 | DB6 | DB5);
	i2c_lcd_write4bits(DB7 | DB6 | DB5 | DB4); // Blinking
	_delay_ms(5); 

	
	// Page42 step 5: set mode to increment address by one 
	// and to shift the cursor to the right. 
	// Display is not shifted. 
	// 0000 0110 
	i2c_lcd_write4bits(0x00);
	i2c_lcd_write4bits(DB6 | DB5);

	_delay_ms(5); 

#if 0 
	// Page42 step 6: Write a H (we don't implement) 
	i2c_lcd_write4bits(RS | DB6);	// Writes H. 
	i2c_lcd_write4bits(RS | DB7);	// shifts to the right
	_delay_ms(4); 
#endif 

	i2c_ioexpander_write(BT);		// Turn LED on 
	_delay_ms(5); 

	i2c_lcd_write(CLEAR_DISPLAY);

#if DEBUG 
	// Print 0 -> 9 test pattern 
	for (i = '0'; i <= '9'; i++) {
		i2c_lcd_putchar(i);
	}
	_delay_ms(600); 

//	i2c_lcd_write(CLEAR_DISPLAY);
	i2c_lcd_write(RETURN_HOME);
	for(j=0;j<40;j++) {
		i2c_lcd_write(0b00010100);
	}
	// Print a to z test pattern 
	for (i = 'a'; i <= 'z'; i++) {
		i2c_lcd_putchar(i);
	}
	_delay_ms(600); 
#endif 

	return; 
}

/* EOF */
