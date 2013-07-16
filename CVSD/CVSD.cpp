#include "CVSD.h"
#include <math.h>

int cvsd_decode(CVSD_STATE_t *state, uint8_t bits)
{
	int			idx;
	int			sample_bigger_than_ref;
	uint16_t	syllabic_diff;
	int16_t		integrator_diff;

	for( idx = 0; idx < (BITRATE/SAMPLERATE); idx++)
	{
		// Extract the comparator output bit
		sample_bigger_than_ref = bits & (1 << ( (BITRATE/SAMPLERATE) - idx - 1)) ? 1 : 0;

		// Add the bit into the shift register
		state->ShiftRegister = ((state->ShiftRegister << 1) | sample_bigger_than_ref) & SR_MASK ;

		//  PROCESS SYLLABIC BLOCK
		// Apply overflow detector - all ones or all zeroes
		if(state->ShiftRegister == 0)				// All zeroes condition detected
		{
			syllabic_diff = (state->V_syllabic * SYLLABIC_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			state->V_syllabic -= syllabic_diff;
			state->V_syllabic = MAX(state->V_syllabic, SYLLABIC_MIN);
		}else if(state->ShiftRegister == SR_MASK)	// All ones condition detected
		{
			syllabic_diff = ((MAX_ABS_DATA - state->V_syllabic) * SYLLABIC_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			state->V_syllabic += syllabic_diff;
		}

		// PROCESS INTEGRATOR BLOCK
		int16_t V_s = sample_bigger_than_ref ? (int32_t)state->V_syllabic : -(int32_t)state->V_syllabic;
		integrator_diff = (( V_s - state->V_integrator) * INTEGRATOR_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
		state->V_integrator += integrator_diff;
	}

	return (int) state->V_integrator;
}

uint8_t cvsd_encode(CVSD_STATE_t *state, int sample)
{
	uint8_t		res;
	int			idx;
	int			sample_bigger_than_ref;
	uint16_t	syllabic_diff;
	int16_t		integrator_diff;

	res = 0;
	for( idx = 0; idx < (BITRATE/SAMPLERATE); idx++)
	{
		// Calculate the comparator output
		sample_bigger_than_ref = sample > state->V_integrator ? 1 : 0;
		
		// Add the bit into the shift register
		state->ShiftRegister = ((state->ShiftRegister << 1) | sample_bigger_than_ref) & SR_MASK ;
		//  .... and into the result
		res = (res << 1) | sample_bigger_than_ref;

		//  PROCESS SYLLABIC BLOCK
		// Apply overflow detector - all ones or all zeroes
		if(state->ShiftRegister == 0)				// All zeroes condition detected
		{
			syllabic_diff = (state->V_syllabic * SYLLABIC_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			state->V_syllabic -= syllabic_diff;
			state->V_syllabic = MAX(state->V_syllabic, SYLLABIC_MIN);
		}else if(state->ShiftRegister == SR_MASK)	// All ones condition detected
		{
			syllabic_diff = ((MAX_ABS_DATA - state->V_syllabic) * SYLLABIC_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			state->V_syllabic += syllabic_diff;
		}

		// PROCESS INTEGRATOR BLOCK
		int16_t V_s = sample_bigger_than_ref ? (int32_t)state->V_syllabic : -(int32_t)state->V_syllabic;
		integrator_diff = (( V_s - state->V_integrator) * INTEGRATOR_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
		state->V_integrator += integrator_diff;
	}

	return res;
}

void cvsd_init(CVSD_STATE_t *state)
{		
	state->ShiftRegister = 0;
	state->V_integrator = 0;
	state->V_syllabic = SYLLABIC_MIN;
}

int			test_vec[400];
uint8_t		test_bits[100];
int			result_vec[400];

#define PI (3.1415926535897932384626433)
#define FREQ  (800)

int cvsd_unit_test()
{
	uint8_t			data;
	unsigned int	bit_count, byte_count;
	// Generate test vector
	for(int i = 0; i < 400; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI)/ (SAMPLERATE * 1.0) ) * MAX_ABS_DATA );
	}

	CVSD_STATE_t	enc;
	cvsd_init(&enc);

	// Encode samples into CVSD stream
	byte_count = bit_count = 0;
	for(int i = 0; i < 400; i++)
	{
		data = cvsd_encode(&enc, test_vec[i]);
		for( int idx = 0; idx < (BITRATE/SAMPLERATE); idx++)
		{
			unsigned int bit = data & (1 << ( (BITRATE/SAMPLERATE) - idx - 1)) ? 1 : 0;
			test_bits[byte_count] |= (bit << (7 - bit_count));
			bit_count++;
			if(bit_count >= 8)
			{	
				byte_count++;
				bit_count = 0;
				test_bits[byte_count] = 0;
			}
		}
	}

	CVSD_STATE_t	dec;
	cvsd_init(&dec);

	for(int i = 0; i < 20; i++)
	{
		// Run of 3 = 0.33
		test_bits[i*5 + 0] = 0xFB;
		test_bits[i*5 + 1] = 0x41;
		test_bits[i*5 + 2] = 0x2F;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x12;

		// Run of 3 = 0
//		test_bits[i*5 + 0] = 0xDB;
//		test_bits[i*5 + 1] = 0x49;
//		test_bits[i*5 + 2] = 0x2D;
//		test_bits[i*5 + 3] = 0xB4;
//		test_bits[i*5 + 4] = 0x92;
	}
	// Decode the received CVSD stream
	byte_count = bit_count = 0;
	for(int i = 0; i < 400; i++)
	{
		data = 0;
		for( int idx = 0; idx < (BITRATE/SAMPLERATE); idx++)
		{
			unsigned int bit = test_bits[byte_count] & (1 << (7 - bit_count)) ? 1 : 0;
			data = (data << 1) | bit;
			bit_count++;
			if(bit_count >= 8)
			{	
				byte_count++;
				bit_count = 0;
			}
		}
		result_vec[i] = cvsd_decode(&dec, data);
	}
	return 0;
}