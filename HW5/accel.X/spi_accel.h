/* 
 * File:   spi_accel.h
 * Author: Lee
 *
 * Created on April 19, 2015, 7:34 PM
 */

#ifndef SPI_ACCEL_H
#define	SPI_ACCEL_H

unsigned char spi_io(unsigned char o);

void acc_read_register(unsigned char reg, unsigned char data[], unsigned int len);

void acc_write_register(unsigned char reg, unsigned char data);

void acc_setup();



#endif	/* SPI_ACCEL_H */

