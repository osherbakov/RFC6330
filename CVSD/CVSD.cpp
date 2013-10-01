#include "CVSD.h"
#include <math.h>
#include <string.h>

void cvsd_init(CVSD_STATE_t *state, CVSD_TYPE_t cvsd_type)
{
	int i;	
	state->ShiftRegister = 0;
	state->V_integrator = 0;
	state->V_syllabic = SYLLABIC_MIN;
	state->n_coeff = N_COEFF;
	for(i = 0; i < N_COEFF; i++)
	{
		state->filt_den[i] = FILT_DEN[i];
		// For interpolation filter L-1 samples will be zeroes
		// Compensate for that by scaling all Numerator coefficients
		state->filt_num[i] = FILT_NUM[i];
		//cvsd_type == CVSD_DEC ? FILT_NUM[i] : 
		//	(FILT_NUM[i] * N_COEFF)/(N_COEFF - (BITRATE_KB/SAMPLERATE_KS) + 1);
	}
	memset(state->filt_states, 0, sizeof(state->filt_states));
}

void cvsd_8K_to_16K(CVSD_STATE_t *state, int *dest, int *src, int src_cnt)
{
	int i;
	for (i = 0; i < src_cnt; i++)
	{
		*dest++ = cvsd_filter(state, *src);
		*dest++ = cvsd_filter(state, 0);
		src++;
	}
}


void cvsd_16K_to_8K(CVSD_STATE_t *state, int *dest, int *src, int src_cnt)
{
	int i;
	for (i = 0; i < src_cnt/2; i++)
	{
		*dest++ = cvsd_filter(state, *src++);
		cvsd_filter(state, *src++);
	}
}

int cvsd_filter(CVSD_STATE_t *state, int sample)
{
	int32_t  mac;
	int16_t *coeffs = &state->filt_den[0];
	int16_t *states = &state->filt_states[0];

	// Process Denominator part
	mac = sample * *coeffs++;
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
	for (int i = N_COEFF-2; i > 0; i--)
	{
		state->filt_states[i] = state->filt_states[i-1];
	}
	state->filt_states[0] = sample;

	return mac >> 12;
}

int cvsd_decode(CVSD_STATE_t *state, uint8_t bit)
{
	uint8_t			sample_bigger_than_ref;
	uint32_t		V_syllabic;
	int32_t			V_integrator;
	int32_t			V_s;
	uint8_t			SR;
	int				result;

	// Get data from the state and put them into work variables
	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	// Extract the comparator output bit
	sample_bigger_than_ref = bit & 0x01;

	// Add the bit into the shift register
	SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK ;

	//  PROCESS SYLLABIC BLOCK
	// Apply overflow detector - all ones or all zeroes
	if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
	{
		V_syllabic += FP_MULT(MAX_DATA - V_syllabic,  SYLLABIC_STEP);
	}else
	{
		V_syllabic -= FP_MULT(V_syllabic - SYLLABIC_MIN, SYLLABIC_LEAK);
	}

	// PROCESS INTEGRATOR BLOCK
	V_s = sample_bigger_than_ref ? (int32_t)V_syllabic : -(int32_t)V_syllabic;
	V_integrator += FP_MULT(V_s * MAX_SLOPE - V_integrator, INTEGRATOR_STEP);
	// Add leakage...
	V_integrator -= FP_MULT(V_integrator, INTEGRATOR_LEAK);

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;

	result = MIN(V_integrator, MAX_DATA);
	result = MAX(result, MIN_DATA);
	return result;
}

void cvsd_decode_buffer(CVSD_STATE_t *state, int16_t *pSamples, uint8_t *pBits, int nBits)
{
	uint8_t			sample_bigger_than_ref;
	uint32_t		V_syllabic;
	int32_t			V_integrator;
	int32_t			V_s;
	uint8_t			SR;
	int				result;
	int				bitcount, databyte;

	// Get data from the state and put them into work variables
	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	bitcount = 8;
	while(nBits--)
	{
		if(++bitcount >= 8)
		{
			databyte = *pBits++;
			bitcount = 0;
		}
		// Extract the comparator output bit
		sample_bigger_than_ref = (databyte >> (7-bitcount)) & 0x01;

		// Add the bit into the shift register
		SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK ;

		//  PROCESS SYLLABIC BLOCK
		// Apply overflow detector - all ones or all zeroes
		if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
		{
			V_syllabic += FP_MULT(MAX_DATA - V_syllabic,  SYLLABIC_STEP);
		}else
		{
			V_syllabic -= FP_MULT(V_syllabic - SYLLABIC_MIN, SYLLABIC_LEAK);
		}

		// PROCESS INTEGRATOR BLOCK
		V_s = sample_bigger_than_ref ? (int32_t)V_syllabic : -(int32_t)V_syllabic;
		V_integrator += FP_MULT(V_s * MAX_SLOPE - V_integrator, INTEGRATOR_STEP);
		// Add leakage...
		V_integrator -= FP_MULT(V_integrator, INTEGRATOR_LEAK);
		result = MIN(V_integrator, MAX_DATA);
		result = MAX(result, MIN_DATA);
		*pSamples++ = result;
	}

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
}

uint8_t cvsd_encode(CVSD_STATE_t *state, int sample)
{
	uint8_t			sample_bigger_than_ref;
	uint32_t		V_syllabic;
	int32_t			V_integrator;
	int32_t			V_s;
	uint8_t			SR;

	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	// Calculate the comparator output
	sample_bigger_than_ref = ((int32_t)sample) > V_integrator ? 0x01 : 0x00;
	
	// Add the comparator bit into the shift register
	SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK ;

	//  PROCESS SYLLABIC BLOCK
	// Apply overflow detector - all ones or all zeroes
	if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
	{
		V_syllabic += FP_MULT(MAX_DATA - V_syllabic, SYLLABIC_STEP);
	}else
	{
		V_syllabic -= FP_MULT(V_syllabic - SYLLABIC_MIN, SYLLABIC_LEAK);
	}

	// PROCESS INTEGRATOR BLOCK
	V_s = sample_bigger_than_ref ? (int32_t)V_syllabic : -(int32_t)V_syllabic;
	V_integrator +=  FP_MULT(V_s * MAX_SLOPE - V_integrator,  INTEGRATOR_STEP);
	// Add leakage...
	V_integrator -= FP_MULT(V_integrator, INTEGRATOR_LEAK);

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
	return sample_bigger_than_ref;
}

void cvsd_encode_buffer(CVSD_STATE_t *state, uint8_t *pBits, int16_t *pSamples, int nSamples)
{
	uint8_t			sample_bigger_than_ref;
	uint32_t		V_syllabic;
	int32_t			V_integrator;
	int32_t			V_s;
	uint8_t			SR;
	int				databyte, bitcount;

	V_syllabic = state->V_syllabic;
	V_integrator = state->V_integrator;
	SR = state->ShiftRegister;

	bitcount = 0;

	while(nSamples--)
	{
		// Calculate the comparator output
		sample_bigger_than_ref = ((int32_t)*pSamples++) > V_integrator ? 0x01 : 0x00;
		
		// Add the comparator bit into the shift register
		SR = ((SR << 1) | sample_bigger_than_ref) & SR_MASK;
		// Add bit into databyte
		databyte = (databyte << 1) | sample_bigger_than_ref;
		if(++bitcount >= 8)
		{
			*pBits++ = databyte;
			bitcount = 0;
		}

		//  PROCESS SYLLABIC BLOCK
		// Apply overflow detector - all ones or all zeroes
		if((SR == 0) || (SR == SR_MASK)) // All zeroes or ones condition detected
		{
			V_syllabic += FP_MULT(MAX_DATA - V_syllabic, SYLLABIC_STEP);
		}else
		{
			V_syllabic -= FP_MULT(V_syllabic - SYLLABIC_MIN, SYLLABIC_LEAK);
		}

		// PROCESS INTEGRATOR BLOCK
		V_s = sample_bigger_than_ref ? (int32_t)V_syllabic : -(int32_t)V_syllabic;
		V_integrator +=  FP_MULT(V_s * MAX_SLOPE - V_integrator,  INTEGRATOR_STEP);
		// Add leakage...
		V_integrator -= FP_MULT(V_integrator, INTEGRATOR_LEAK);
	}

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
}




	CVSD_STATE_t	enc;
	CVSD_STATE_t	dec;

int			result_vec[2000];
int			result_vec_8K[1000];
int			test_vec_8K[1000];
int			test_vec[2000];
uint8_t		test_bits[250];
uint8_t		databits[250];
int16_t		samples[2000];




int main()
{

	uint8_t			databit, databyte;
	unsigned int	bit_count, byte_count;

	cvsd_init(&enc, CVSD_ENC);

	fill_8K_buffer(test_vec_8K);
	cvsd_8K_to_16K(&enc, test_vec, test_vec_8K, 1000);

	fill_16K_buffer(test_vec);

cvsd_init(&enc, CVSD_ENC);
fill_samples_buffer(samples);
cvsd_encode_buffer(&enc, databits, samples, 2000);


	cvsd_init(&enc, CVSD_ENC);
	// Encode samples into CVSD stream
	byte_count = bit_count = 0;
	databyte = 0;
	for(int i = 0; i < 2000; i++)
	{
		databit = cvsd_encode(&enc, test_vec[i]);
		databyte = (databyte << 1)| databit;
		if(++bit_count >= 8)
		{	
			test_bits[byte_count++] = databyte;
			bit_count = 0;
		}
	}

//--------------------------------------------------------
//	fill_data(test_bits, 0);
//--------------------------------------------------------

	cvsd_init(&dec, CVSD_DEC);

	// Decode the received CVSD stream into regular samples
	byte_count = bit_count = 0;
	databyte = test_bits[byte_count++];
	for(int i = 0; i < 2000; i++)
	{
		databit = databyte & (1 << (7 - bit_count)) ? 1 : 0;
		result_vec[i] = cvsd_decode(&dec, databit);
		bit_count++;
		if(bit_count >= 8)
		{	
			databyte = test_bits[byte_count++];
			bit_count = 0;
		}
	}
	cvsd_16K_to_8K(&dec, result_vec_8K, result_vec, 2000);

	cvsd_init(&dec, CVSD_DEC);
	cvsd_decode_buffer(&dec, samples, databits, 2000);
}