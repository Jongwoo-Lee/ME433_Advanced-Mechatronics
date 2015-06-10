
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "hidapi.h"

#define MAX_STR 255
#define DATAPNTS 200

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	
	int i;
	
	// Initialize the hidapi library
	res = hid_init();

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x4d8, 0x3f, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	wprintf(L"Product String: %s\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	wprintf(L"Serial Number String: (%d) %s\n", wstr[0], wstr);

	// Read Indexed String 1
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	wprintf(L"Indexed String 1: %s\n", wstr);

	// Toggle LED (cmd 0x80). The first byte is the report number (0x0).
	buf[0] = 0x0;
	buf[1] = 0x80;
	res = hid_write(handle, buf, 65);



	unsigned char message[27];
	//Get user input with scanf ; where message is a character array
	buf[1] = 0x82;
	printf("%s", "Please enter the row number : ");
	scanf("%s", &buf[2]);
	printf("%s", "Please enter a sentence : ");
	scanf("%s", message);
	
	for (i = 0; i < 27; i++)
	{
		buf[i+3] = message[i];
	}
	
	res = hid_write(handle, buf, 65);
	
	
	// Request state (cmd 0x81). The first byte is the report number (0x0).

	buf[0] = 0x0;
	buf[1] = 0x81;
	res = hid_write(handle, buf, 65);

	// Read requested state
	res = hid_read(handle, buf, 65);
	
	
	
	
/*        
    short accels[3];
    
	accels[0] = (buf[2]<<8)|(buf[1]&0xff);
    accels[1] = (buf[4]<<8)|(buf[3]&0xff);
    accels[2] = (buf[6]<<8)|(buf[5]&0xff);
    
    for(i = 0; i<3;i++)
    	wprintf(L"accel[%d]: %d\n",i,accels[i]);

	short x,y,z;
	x = (buf[4]<<8)|(buf[3]);
	y = (buf[6]<<8)|(buf[5]);
	z = (buf[8]<<8)|(buf[7]);
	
	double xx,yy,zz;
	xx=((float)x)/16000;
	yy=((float)y)/16000;
	zz=((float)z)/16000;
	
	// Print out the returned buffer.
	printf("%f \n %f \n %f \n", xx, yy, zz);
	*/

	short z[DATAPNTS];
	double time = 0;
	double t[DATAPNTS];
	FILE *ofp;
	
	ofp = fopen("accels.txt", "w");
	
	for (i = 0; i < DATAPNTS ; i ++ ) {
		
		
		
		res = hid_write(handle, buf, 65);
		res = hid_read(handle, buf, 65);
		
    	z[i] = (buf[6]<<8)|(buf[5]&0xff);
    	
		t[i] = time;
		time = time + 0.04;
		
		fprintf(ofp, "%d	%f\r\n", z[i],t[i]);
	}
	
	fclose(ofp);
	

	// Finalize the hidapi library
	res = hid_exit();

	return 0;
}
