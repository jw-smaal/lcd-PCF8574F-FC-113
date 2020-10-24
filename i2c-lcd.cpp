/**
 * LCD with I2C wing attached at address 0x4E (write) 0x4F (read) 
 *
 * Jan-Willem Smaal <usenet@gispen.org> 5 June 2020
 *******************************************************************
 */
#include <inttypes.h>
#include "mbed.h"

/**
 * I2C LCD interface using io expander (PCF8574F) wing FC-113. 
 * by Jan-Willem Smaal <usenet@gispen.org> 
 */  
#include "i2c-lcd.h"
I2C i2c(I2C_SDA, I2C_SCL);
#define DEBUG 1 

/**
 * Moves the cursor 40 positions to the right
 * so it ends up on line2.
 */
void I2cLcd::move_cursor_line2(void){
	uint8_t j; 

	I2cLcd::write(RETURN_HOME);
	for(j=0;j<40;j++) {
		I2cLcd::write(0b00010100);
	}

	return;
} 
	

/**
 * Puts the cursor back at line2 
 */ 
void I2cLcd::move_cursor_line1(void){
	I2cLcd::write(RETURN_HOME);

	return;
} 
	


/**
 * Takes care of sending the I2C start stop to the correct
 * IO expander.  
 */ 
void i2c_ioexpander_write(uint8_t value) 
{
	i2c.start(); 
	i2c.write(PCF8574T_WRITE);
	i2c.write(value);
	i2c.stop();

	return;
}


/**
 * Takes care of sending the 8 bit byte 
 * with the enable pin pulsed (required for 4 bit mode)     
 */
void I2cLcd::write4bits(uint8_t val) 
{
    i2c_ioexpander_write(val);
	pulse_enable(val);

	return;
}


/**
 * in 4 bits mode we need to pulse the bytes in.  
 * one nibble at a time. 
 */
void I2cLcd::write(uint8_t val) 
{
	uint8_t lnibble, hnibble; 
	hnibble = val & 0xF0;	// High nibble 4 bits
	lnibble = val & 0x0F; // Low  nibble 4 bits 
	I2cLcd::write4bits(hnibble);	
	I2cLcd::write4bits(lnibble<<4);	
	
	return;
}



/**
 * Output a character in 4 bit mode. 
 */ 
void I2cLcd::putchar(char c) 
{
	//I2cLcd::write((uint8_t) c);


	uint8_t lnibble, hnibble; 
	hnibble = c & 0xF0;	// High nibble 4 bits
	lnibble = c & 0x0F; // Low  nibble 4 bits 
	I2cLcd::write4bits(RS | BT | hnibble);	
	I2cLcd::write4bits(RS | BT | lnibble<<4);	

	return;
}


/**
 * Toggle enable pin
 */
void I2cLcd::pulse_enable(uint8_t val)
{
    i2c_ioexpander_write((val|BT) | EN);		// Enable  high
    ThisThread::sleep_for(1ms);						// >450ns

	i2c_ioexpander_write((val|BT) & ~EN);	// Enable low
    ThisThread::sleep_for(1ms);						// > 37us e

	return; 
}


/** Constructor 
 * We init the LCD in 4 bit mode. 
 *
 * Just following the Hitachi datasheet. 
 * exactly as written no shortcuts.  
 */ 
 I2cLcd::I2cLcd()
{
	uint8_t i, j, lnibble, hnibble; 

	// We really need to wait a long time 
	// to ensure the LCD is ready after powerup. 
	ThisThread::sleep_for(600ms);
	i2c_ioexpander_write(~BT);
	
#if 0  
	// !! 
	I2cLcd::write4bits(DB6| DB5);
	ThisThread::sleep_for(5); 
	// !! 
	I2cLcd::write4bits(DB6| DB5);
	ThisThread::sleep_for(5); 
	// !! 
	I2cLcd::write4bits(DB6| DB5);
	ThisThread::sleep_for(5); 
#endif 


	// page42 Step 2 -> 4 bit mode. 
	I2cLcd::write4bits(DB5);
	ThisThread::sleep_for(5ms); 

	// page42 Step 3a
	I2cLcd::write4bits(DB5);	// One line. 
	ThisThread::sleep_for(5ms); 


	// page42 Step 3b: set 4 bit operation and select 1 line displays
	// DB5 == function set 
	I2cLcd::write4bits(DB5);
	ThisThread::sleep_for(150ms);


	/* 
	 * !! From this point on 4 bit operation is used !! 
	 */ 
	//I2cLcd::write(0b00101100);
	I2cLcd::write(DB5 | DB3 | DB2);
	ThisThread::sleep_for(150ms); 



	// page42 Step 4: Turn on display and cursor so 0000 1111 
	// which means DB0,1,2,3 actually.  
	I2cLcd::write4bits(0x00);
	//I2cLcd::write4bits(DB7 | DB6 | DB5);
	I2cLcd::write4bits(DB7 | DB6 | DB5 | DB4); // Blinking
	ThisThread::sleep_for(5ms); 

	
	// Page42 step 5: set mode to increment address by one 
	// and to shift the cursor to the right. 
	// Display is not shifted. 
	// 0000 0110 
	I2cLcd::write4bits(0x00);
	I2cLcd::write4bits(DB6 | DB5);

	ThisThread::sleep_for(5ms); 

#if 0 
	// Page42 step 6: Write a H (we don't implement) 
	I2cLcd::write4bits(RS | DB6);	// Writes H. 
	I2cLcd::write4bits(RS | DB7);	// shifts to the right
	ThisThread::sleep_for(4); 
#endif 

	i2c_ioexpander_write(BT);		// Turn LED on 
	ThisThread::sleep_for(5ms); 

	I2cLcd::write(CLEAR_DISPLAY);

#if DEBUG 
	// Print 0 -> 9 test pattern 
	for (i = '0'; i <= '9'; i++) {
		I2cLcd::putchar(i);
	}
	ThisThread::sleep_for(600ms); 

//	I2cLcd::write(CLEAR_DISPLAY);
	I2cLcd::write(RETURN_HOME);
	for(j=0;j<40;j++) {
		I2cLcd::write(0b00010100);
	}
	// Print a to z test pattern 
	for (i = 'a'; i <= 'z'; i++) {
		I2cLcd::putchar(i);
	}
	ThisThread::sleep_for(600ms); 
#endif 
 
}

/* EOF */
