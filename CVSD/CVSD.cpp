#include "CVSD.h"
#include <math.h>

int cvsd_decode(CVSD_STATE_t *state, uint8_t bits)
{
	int			idx;
	int			sample_bigger_than_ref;
	unsigned int	V_syllabic, syllabic_diff;
	int				V_integrator, integrator_diff;
	int				V_s;
	unsigned int	SR;

	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	for( idx = 0; idx < (BITRATE/SAMPLERATE); idx++)
	{
		// Extract the comparator output bit
		sample_bigger_than_ref = bits & (1 << ( (BITRATE/SAMPLERATE) - idx - 1)) ? 1 : 0;

		// Add the bit into the shift register
		SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK ;

		//  PROCESS SYLLABIC BLOCK
		// Apply overflow detector - all ones or all zeroes
		if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
		{
			syllabic_diff = ((MAX_DATA - V_syllabic) * SYLLABIC_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			V_syllabic += syllabic_diff;
			V_syllabic = MIN(V_syllabic, MAX_DATA);
		}else
		{
			syllabic_diff = (state->V_syllabic * SYLLABIC_LEAK + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			V_syllabic -= syllabic_diff;
			V_syllabic = MAX(V_syllabic, SYLLABIC_MIN);
		}

		// PROCESS INTEGRATOR BLOCK
		V_s = sample_bigger_than_ref ? (int)V_syllabic : -(int)V_syllabic;
		integrator_diff = (( V_s - V_integrator) * INTEGRATOR_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
		// Add leakage...
		integrator_diff -= (V_integrator * INTEGRATOR_LEAK + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
		V_integrator += integrator_diff;
		V_integrator = MIN(V_integrator, MAX_DATA);
		V_integrator = MAX(V_integrator, MIN_DATA);
	}
	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
	return (int) V_syllabic;
}

uint8_t cvsd_encode(CVSD_STATE_t *state, int sample)
{
	uint8_t		res;
	int			idx;
	int			sample_bigger_than_ref;
	unsigned int	V_syllabic, syllabic_diff;
	int				V_integrator, integrator_diff;
	int				V_s;
	unsigned int	SR;

	res = 0;
	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	for( idx = 0; idx < (BITRATE/SAMPLERATE); idx++)
	{
		// Calculate the comparator output
		sample_bigger_than_ref = sample > V_integrator ? 1 : 0;
		
		// Add the comparator bit into the shift register
		SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK ;
		//  .... and into the result
		res = (res << 1) | sample_bigger_than_ref;

		//  PROCESS SYLLABIC BLOCK
		// Apply overflow detector - all ones or all zeroes
		if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
		{
			syllabic_diff = ((MAX_DATA - V_syllabic) * SYLLABIC_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			V_syllabic += syllabic_diff;
			V_syllabic = MIN(V_syllabic, MAX_DATA);
		}else
		{
			syllabic_diff = (V_syllabic * SYLLABIC_LEAK + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
			V_syllabic -= syllabic_diff;
			V_syllabic = MAX(V_syllabic, SYLLABIC_MIN);
		}

		// PROCESS INTEGRATOR BLOCK
		V_s = sample_bigger_than_ref ? (int)V_syllabic : -(int)V_syllabic;
		integrator_diff = (( V_s - V_integrator) * INTEGRATOR_STEP + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
		// Add leakage...
		integrator_diff -= (V_integrator * INTEGRATOR_LEAK + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT;
		V_integrator += integrator_diff;
		V_integrator = MIN(V_integrator, MAX_DATA);
		V_integrator = MAX(V_integrator, MIN_DATA);
	}
	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
	return res;
}

int			test_vec[400];
uint8_t		test_bits[100];
int			result_vec[400];

#define PI (3.1415926535897932384626433)
#define TWO_PI (2.0 * PI)
#define FREQ  (800)

int main()
{
	uint8_t			data;
	unsigned int	bit_count, byte_count;
	// Generate test vector
	for(int i = 0; i < 100; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI/4)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 100; i < 200; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * TWO_PI)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 200; i < 300; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 300; i < 400; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI/2)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	CVSD_STATE_t	enc;
	enc.ShiftRegister = 0;
	enc.V_integrator = 0;
	enc.V_syllabic = SYLLABIC_MIN;


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

//--------------------------------------------------------

/***************************/
	for(int i = 0; i < 100; i++)
	{
		test_bits[i] = 0x00;
	}

	for(int i = 0; i < 4; i++)
	{
		// Run of 3 = 0
		test_bits[i*5 + 0] = 0xDB;
		test_bits[i*5 + 1] = 0x49;
		test_bits[i*5 + 2] = 0x2D;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x92;
	}

	for(int i = 4; i < 12; i++)
	{
		// Run of 3 = 0.33
		test_bits[i*5 + 0] = 0xFB;
		test_bits[i*5 + 1] = 0x41;
		test_bits[i*5 + 2] = 0x2F;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x12;
	}
	for(int i = 12; i < 20; i++)
	{
		// Run of 3 = 0
		test_bits[i*5 + 0] = 0xDB;
		test_bits[i*5 + 1] = 0x49;
		test_bits[i*5 + 2] = 0x2D;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x92;
	}
/******************************/

//--------------------------------------------------------

	CVSD_STATE_t	dec;
	dec.ShiftRegister = 0;
	dec.V_integrator = 0;
	dec.V_syllabic = SYLLABIC_MIN;

	// Decode the received CVSD stream into regular samples
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

}