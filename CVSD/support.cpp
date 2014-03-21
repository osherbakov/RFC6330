#include "CVSD.h"
#include <math.h>
#include <string.h>

#define PI (3.1415926535897932384626433)
#define TWO_PI (2.0 * PI)
#define FREQ  (3200)

static uint8_t run_0_3[] = {0xFB, 0x41, 0x2F, 0xB4, 0x12};
static uint8_t run_0_0[] = {0xDB, 0x49, 0x2D, 0xB4, 0x92};


void fill_test_data(uint8_t *pData, int insert_offset, int run_size_bytes, int num_bytes)
{
	int curr_run_size;
	int i;

	pData += insert_offset;

	while(num_bytes > 0)
	{
		//// Run of alternating 0'es and 1's - almost DC
		//run_size_bytes = MIN(run_size_bytes, num_bytes);
		//for(i = 0; i < run_size_bytes; i++ )
		//{
		//	*pData++ = 0x55;
		//}
		//num_bytes -= run_size_bytes;
		
		// Run of 0.3 all 1's and 0'es
		run_size_bytes = MIN(run_size_bytes, num_bytes);
		for(i = 0; i < run_size_bytes; i++ )
		{
			*pData++ = run_0_3[i % 5];
		}
		num_bytes -= run_size_bytes;

		// Run of 0.0 all 1's and 0'es
		run_size_bytes = MIN(run_size_bytes, num_bytes);
		for(i = 0; i < run_size_bytes; i++ )
		{
			*pData++ = run_0_0[i % 5];
		}
		num_bytes -= run_size_bytes;

		// Run of alternating 0'es and 1's - almost DC
		run_size_bytes = MIN(run_size_bytes, num_bytes);
		for(i = 0; i < run_size_bytes; i++ )
		{
			*pData++ = 0x55;
		}
		num_bytes -= run_size_bytes;

	}
}