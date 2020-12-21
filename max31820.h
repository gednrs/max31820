#ifndef _MAX31820_H_  
#define _MAX31820_H_

void max31820_send(char bit);
unsigned char max31820_read(void);
void max31820_send_byte(char data);
unsigned char max31820_read_byte(void);
unsigned char max31820_send_reset_pulse(void);
float max31820_get_temp(void);
void max31820_init();

#endif
