#include "CVSD.h"
#include <math.h>
#include <string.h>

void cvsd_init(CVSD_STATE_t *state)
{
	state->ShiftRegister = 0;
	state->V_integrator = 0;
	state->V_syllabic = SYLLABIC_MIN;
	memcpy(state->filt_num, DEC_NUM, N_COEFF * sizeof(state->filt_num[0]));
	memcpy(state->filt_den, DEC_DEN, N_COEFF * sizeof(state->filt_den[0]));
	memset(state->filt_states, 0, (N_COEFF-1) * sizeof(state->filt_states[0]));
}

int cvsd_filter(CVSD_STATE_t *state, int sample)
{
	int32_t  mac;
	int16_t *coeffs = &state->filt_den[0];
	int16_t *states = &state->filt_states[0];

	// Process Denominator part
	mac = *coeffs++ * sample;
	for(int i = 0; i < (N_COEFF-1); i++)
	{
		mac-= (*coeffs++ * *states++);
	}

	sample = mac >> 15;

	// Process Nominator part
	coeffs = &state->filt_num[0];
	states = &state->filt_states[0];
	mac = *coeffs++ * sample;
	for(int i = 0; i < (N_COEFF-1); i++)
	{
		mac+= (*coeffs++ * *states++);
	}
	// shift the delay line and add new sample
	for (int i = N_COEFF-1; i > 0; i--)	state->filt_states[i] = state->filt_states[i-1];
	state->filt_states[0] = sample;

	return mac >> 12;
}

int cvsd_decode(CVSD_STATE_t *state, uint8_t bits)
{
	int				sample_bigger_than_ref;
	unsigned int	V_syllabic, syllabic_diff;
	int				V_integrator, integrator_diff;
	int				V_s;
	unsigned int	SR;

	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	// Extract the comparator output bit
	sample_bigger_than_ref = bits & 0x01;

	// Add the bit into the shift register
	SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK ;

	//  PROCESS SYLLABIC BLOCK
	// Apply overflow detector - all ones or all zeroes
	if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
	{
		syllabic_diff = MULT(MAX_DATA - V_syllabic,  SYLLABIC_STEP);
		V_syllabic += syllabic_diff;
	}else
	{
		syllabic_diff = MULT(state->V_syllabic - SYLLABIC_MIN,SYLLABIC_LEAK);
		V_syllabic -= syllabic_diff;
	}

	// PROCESS INTEGRATOR BLOCK
	V_s = sample_bigger_than_ref ? (int)V_syllabic : -(int)V_syllabic;
	integrator_diff = MULT(V_s*MAX_SLOPE - V_integrator, INTEGRATOR_STEP);
	// Add leakage...
	integrator_diff -= MULT(V_integrator, INTEGRATOR_LEAK);
	V_integrator += integrator_diff;
	V_integrator = MIN(V_integrator, MAX_DATA);
	V_integrator = MAX(V_integrator, MIN_DATA);

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
	return (int) V_integrator;
}

uint8_t cvsd_encode(CVSD_STATE_t *state, int sample)
{
	int				sample_bigger_than_ref;
	unsigned int	V_syllabic, syllabic_diff;
	int				V_integrator, integrator_diff;
	int				V_s;
	unsigned int	SR;

	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	// Calculate the comparator output
	sample_bigger_than_ref = sample > V_integrator ? 0x01 : 0x00;
	
	// Add the comparator bit into the shift register
	SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK ;

	//  PROCESS SYLLABIC BLOCK
	// Apply overflow detector - all ones or all zeroes
	if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
	{
		syllabic_diff = MULT(MAX_DATA - V_syllabic, SYLLABIC_STEP);
		V_syllabic += syllabic_diff;
	}else
	{
		syllabic_diff = MULT(state->V_syllabic - SYLLABIC_MIN, SYLLABIC_LEAK);
		V_syllabic -= syllabic_diff;
	}

	// PROCESS INTEGRATOR BLOCK
	V_s = sample_bigger_than_ref ? (int)V_syllabic : -(int)V_syllabic;
	integrator_diff = MULT(V_s*MAX_SLOPE - V_integrator,  INTEGRATOR_STEP);
	// Add leakage...
	integrator_diff -= MULT(V_integrator, INTEGRATOR_LEAK);
	V_integrator += integrator_diff;
	V_integrator = MIN(V_integrator, MAX_DATA);
	V_integrator = MAX(V_integrator, MIN_DATA);

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
	return sample_bigger_than_ref;
}

int			test_vec[1000];
uint8_t		test_bits[250];
int			result_vec[2000];

#define PI (3.1415926535897932384626433)
#define TWO_PI (2.0 * PI)
#define FREQ  (800)

int main()
{
	uint8_t			databit, databyte;
	unsigned int	bit_count, byte_count;
	// Generate test vector
	for(int i = 0; i < 400; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * TWO_PI)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 400; i < 400; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 400; i < 600; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI/2)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	for(int i = 600; i < 800; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI/4)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}
	for(int i = 800; i < 1000; i++)
	{
		test_vec[i] = (int) (sin((i * FREQ * PI/8)/ (SAMPLERATE * 1.0) ) * MAX_DATA );
	}

	CVSD_STATE_t	enc;
	cvsd_init(&enc);


	// Encode samples into CVSD stream
	byte_count = bit_count = 0;
	databyte = 0;
	for(int i = 0; i < 1000; i++)
	{
		databit = cvsd_encode(&enc, test_vec[i]);
		databyte = (databyte << 1)| databit;
		bit_count++;
		if(bit_count >= 8)
		{	
			test_bits[byte_count++] = databyte;
			databyte = 0;
			bit_count = 0;
		}
	}

//--------------------------------------------------------

/***************************/

	for(int i = 25 + 0; i < 25 + 5; i++)
	{
		// Run of 3 = 0
		test_bits[i*5 + 0] = 0xDB;
		test_bits[i*5 + 1] = 0x49;
		test_bits[i*5 + 2] = 0x2D;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x92;
	}

	for(int i = 25 + 5; i < 25 + 10; i++)
	{
		// Run of 3 = 0.33
		test_bits[i*5 + 0] = 0xFB;
		test_bits[i*5 + 1] = 0x41;
		test_bits[i*5 + 2] = 0x2F;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x12;
	}

	for(int i = 25 + 10; i < 25 + 15; i++)
	{
		// Run of 3 = 0.33
		test_bits[i*5 + 0] = 0xFB;
		test_bits[i*5 + 1] = 0x41;
		test_bits[i*5 + 2] = 0x2F;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x12;
	}
	for(int i = 25 + 15; i < 25 + 20; i++)
	{
		// Run of 3 = 0
		test_bits[i*5 + 0] = 0xDB;
		test_bits[i*5 + 1] = 0x49;
		test_bits[i*5 + 2] = 0x2D;
		test_bits[i*5 + 3] = 0xB4;
		test_bits[i*5 + 4] = 0x92;
	}
	for(int i = 25 + 20; i < 25 + 25; i++)
	{
		// Run of 3 = 0
		test_bits[i*5 + 0] = 0xdb;
		test_bits[i*5 + 1] = 0x49;
		test_bits[i*5 + 2] = 0x2d;
		test_bits[i*5 + 3] = 0xb4;
		test_bits[i*5 + 4] = 0x92;
	}

/******************************/
//--------------------------------------------------------

	CVSD_STATE_t	dec;
	cvsd_init(&dec);

	// Decode the received CVSD stream into regular samples
	byte_count = bit_count = 0;
	databyte = test_bits[byte_count++];
	for(int i = 0; i < 2000; i++)
	{
		databit = databyte & (1 << (7 - bit_count)) ? 1 : 0;
		result_vec[i] = cvsd_filter(&dec, cvsd_decode(&dec, databit) );
		bit_count++;
		if(bit_count >= 8)
		{	
			databyte = test_bits[byte_count++];
			bit_count = 0;
		}
	}
}