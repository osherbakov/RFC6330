#include "CVSD.h"
#include <math.h>
#include <string.h>

#define PI (3.1415926535897932384626433)
#define TWO_PI (2.0 * PI)
#define FREQ  (3200)


void fill_8K_buffer(int *pBuffer)
{
	// Generate test vector
	for(int i = 0; i < 200; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * TWO_PI)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 200; i < 400; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 400; i < 600; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI/2)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 600; i < 800; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI/4)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}
	for(int i = 800; i < 1000; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI/8)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}
}
void fill_samples_buffer(int16_t *pBuffer)
{
	// Generate test vector
	for(int i = 0; i < 400; i++)
	{
		pBuffer[i] = (int16_t) (sin((i * FREQ * TWO_PI)/ (BITRATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 400; i < 800; i++)
	{
		pBuffer[i] = (int16_t) (sin((i * FREQ * PI)/ (BITRATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 800; i < 1200; i++)
	{
		pBuffer[i] = (int16_t) (sin((i * FREQ * PI/2)/ (BITRATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 1200; i < 1600; i++)
	{
		pBuffer[i] = (int16_t) (sin((i * FREQ * PI/4)/ (BITRATE * 1.0) ) * MAX_DATA );
	}
	for(int i = 1600; i < 2000; i++)
	{
		pBuffer[i] = (int16_t) (sin((i * FREQ * PI/8)/ (BITRATE * 1.0) ) * MAX_DATA );
	}
}


void fill_16K_buffer(int *pBuffer)
{
	for(int i = 0; i < 400; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * TWO_PI)/ (BITRATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 400; i < 800; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI)/ (BITRATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 800; i < 1200; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI/2)/ (BITRATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 1200; i < 1600; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI/4)/ (BITRATE * 1.0) ) * MAX_DATA );
	}
	for(int i = 1600; i < 2000; i++)
	{
		pBuffer[i] = (int) (sin((i * FREQ * PI/8)/ (BITRATE * 1.0) ) * MAX_DATA );
	}
}

void fill_data(uint8_t *pData, int insert_offset)
{

	for(int i = insert_offset + 0; i < insert_offset + 5; i++)
	{
		// Run of 3 = 0
		pData[i*5 + 0] = 0xDB;
		pData[i*5 + 1] = 0x49;
		pData[i*5 + 2] = 0x2D;
		pData[i*5 + 3] = 0xB4;
		pData[i*5 + 4] = 0x92;
	}

	for(int i = insert_offset + 5; i < insert_offset + 10; i++)
	{
		// Run of 3 = 0.33
		pData[i*5 + 0] = 0xFB;
		pData[i*5 + 1] = 0x41;
		pData[i*5 + 2] = 0x2F;
		pData[i*5 + 3] = 0xB4;
		pData[i*5 + 4] = 0x12;
	}

	for(int i = insert_offset + 10; i < insert_offset + 15; i++)
	{
		// Run of 3 = 0.33
		pData[i*5 + 0] = 0xFB;
		pData[i*5 + 1] = 0x41;
		pData[i*5 + 2] = 0x2F;
		pData[i*5 + 3] = 0xB4;
		pData[i*5 + 4] = 0x12;
	}
	for(int i = insert_offset + 15; i < insert_offset + 20; i++)
	{
		// Run of 3 = 0
		pData[i*5 + 0] = 0xDB;
		pData[i*5 + 1] = 0x49;
		pData[i*5 + 2] = 0x2D;
		pData[i*5 + 3] = 0xB4;
		pData[i*5 + 4] = 0x92;
	}
	for(int i = insert_offset + 20; i < insert_offset + 25; i++)
	{
		// Run of 3 = 0
		pData[i*5 + 0] = 0xdb;
		pData[i*5 + 1] = 0x49;
		pData[i*5 + 2] = 0x2d;
		pData[i*5 + 3] = 0xb4;
		pData[i*5 + 4] = 0x92;
	}
}