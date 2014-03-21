#include "CVSD.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

void cvsd_8K_to_16K(CVSD_STATE_t *state, int16_t *dest, int16_t *src, int src_cnt)
{
	int i;
	for (i = 0; i < src_cnt; i++)
	{
		*dest++ = cvsd_filter(state, *src);
		*dest++ = cvsd_filter(state, 0);
		src++;
	}
}


void cvsd_16K_to_8K(CVSD_STATE_t *state, int16_t *dest, int16_t *src, int src_cnt)
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
		mac -= (*coeffs++ * *states++);
	}

	sample = mac >> 15;

	// Process Nominator part
	coeffs = &state->filt_num[0];
	states = &state->filt_states[0];
	mac = *coeffs++ * sample;
	for(int i = 0; i < (N_COEFF-1); i++)
	{
		mac += (*coeffs++ * *states++);
	}
	// shift the delay line and add new sample
	for (int i = N_COEFF-2; i > 0; i--)
	{
		state->filt_states[i] = state->filt_states[i-1];
	}
	state->filt_states[0] = sample;

	return mac >> 12;
}


void cvsd_decode_buffer(CVSD_STATE_t *state, int16_t *pSamples, uint8_t *pBits, int nBits)
{
	uint8_t			sample_bigger_than_ref;
	uint32_t		V_syllabic;
	int32_t			V_integrator, result;
	int32_t			V_s;
	uint8_t			SR;
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
		// V_syllabic = MAX(V_syllabic, SYLLABIC_MIN);

		// PROCESS INTEGRATOR BLOCK
		V_s = sample_bigger_than_ref ? (int32_t)V_syllabic : -(int32_t)V_syllabic;
		V_integrator += FP_MULT(V_s * MAX_SLOPE - V_integrator, INTEGRATOR_STEP);
		// - FP_MULT(V_integrator, INTEGRATOR_LEAK);
		// Add leakage.. 
		V_integrator -= FP_MULT(V_integrator, INTEGRATOR_LEAK);

		//Limit the result
		result = MIN(V_integrator, MAX_DATA);
		result = MAX(result, MIN_DATA);
		*pSamples++ = result;
	}

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
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
		// V_syllabic = MAX(V_syllabic, SYLLABIC_MIN);

		// PROCESS INTEGRATOR BLOCK
		V_s = sample_bigger_than_ref ? (int32_t)V_syllabic : -(int32_t)V_syllabic;
		V_integrator +=  FP_MULT(V_s * MAX_SLOPE - V_integrator,  INTEGRATOR_STEP);
		//- FP_MULT(V_integrator, INTEGRATOR_LEAK);
		// Add leakage...
		V_integrator -= FP_MULT(V_integrator, INTEGRATOR_LEAK);
	}
	if(bitcount > 0)
	{
		*pBits = databyte;
		bitcount = 0;
	}

	state->ShiftRegister = SR;
	state->V_syllabic = V_syllabic;
	state->V_integrator = V_integrator;
}




	CVSD_STATE_t	enc;
	CVSD_STATE_t	dec;


#define PROGRAM_NAME			"CVSD 16000 bps speech coder"
#define PROGRAM_VERSION			"Version 1.0"
#define PROGRAM_DATE			"27 FEB 2014"
#define BLOCK_SIZE				4000
#define TRUE			true
#define FALSE			false
/* ========== Static Variables ========== */

char in_name[256], out_name[256];


/* ========== Local Private Prototypes ========== */

static void		parseCommandLine(int argc, char *argv[]);
static void		printHelpMessage(char *argv[]);

/****************************************************************************
**
** Function:        main
**
** Description:     The main function of the speech coder
**
** Arguments:
**
**  int     argc    ---- number of command line parameters
**  char    *argv[] ---- command line parameters
**
** Return value:    None
**
*****************************************************************************/
int main(int argc, char *argv[])
{
	int32_t		length;
	int16_t		speech_in[BLOCK_SIZE], speech_out[BLOCK_SIZE];
	uint8_t		bitstream[BLOCK_SIZE/8];
                                          /* size of the bitstream buffer */
	bool		eof_reached = false;
	FILE		*fp_in, *fp_out;

	/* ====== Get input parameters from command line ====== */
	parseCommandLine(argc, argv);

	/* ====== Open input, output, and parameter files ====== */
	if ((fp_in = fopen(in_name,"rb")) == NULL){
		fprintf(stderr, "  ERROR: cannot read file %s.\n", in_name);
		exit(1);
	}
	if ((fp_out = fopen(out_name,"wb")) == NULL){
		fprintf(stderr, "  ERROR: cannot write file %s.\n", out_name);
		exit(1);
	}

	/* ====== Initialize CVSD analysis and synthesis ====== */		
	cvsd_init(&enc, CVSD_ENC);
	cvsd_init(&dec, CVSD_DEC);

	/* ====== Run CVSD coder on input signal ====== */

	eof_reached = false;
	while (!eof_reached)
	{

		length = fread(speech_in, sizeof(int16_t), BLOCK_SIZE, fp_in);

		cvsd_encode_buffer(&enc, bitstream, speech_in, length);	
//     fill_test_data(bitstream, 0, (BLOCK_SIZE/8) / 3, BLOCK_SIZE/8);
		cvsd_decode_buffer(&dec, speech_out, bitstream, length);

		fwrite(speech_out, sizeof(int16_t), length, fp_out);
		if (length < BLOCK_SIZE)
		{
			eof_reached = TRUE;
			continue;
		}
	}

	fclose(fp_in);
	fclose(fp_out);
	fprintf(stderr, "\n\n");

	return(0);
}


/****************************************************************************
**
** Function:        parseCommandLine
**
** Description:     Translate command line parameters
**
** Arguments:
**
**  int     argc    ---- number of command line parameters
**  char    *argv[] ---- command line parameters
**
** Return value:    None
**
*****************************************************************************/
static void		parseCommandLine(int argc, char *argv[])
{
	bool		error_flag = FALSE;

	if (argc != 3)
		error_flag = TRUE;

	/* Setting default values. */
	in_name[0] = '\0';
	out_name[0] = '\0';

	if( argc == 3 ){
		strcpy(in_name, argv[1]);
		strcpy(out_name, argv[2]);
	}

    if ((in_name[0] == '\0') || (out_name[0] == '\0'))
		error_flag = TRUE;

	if (error_flag){
		printHelpMessage(argv);
		exit(1);
	}

	fprintf(stderr, "\n\n\t%s %s, %s\n\n", PROGRAM_NAME, PROGRAM_VERSION,
			PROGRAM_DATE);

	fprintf(stderr, " ---- Analysis and Synthesis.\n");

	fprintf(stderr, " ---- input from %s.\n", in_name);
	fprintf(stderr, " ---- output to %s.\n", out_name);
}


/****************************************************************************
**
** Function:        printHelpMessage
**
** Description:     Print Command Line Usage
**
** Arguments:
**
** Return value:    None
**
*****************************************************************************/
static void		printHelpMessage(char *argv[])
{
	fprintf(stderr, "\n\n\t%s %s, %s\n\n", PROGRAM_NAME, PROGRAM_VERSION,
			PROGRAM_DATE);
	fprintf(stdout, "Usage:\n");
	fprintf(stdout, "\t%s infile outfile\n\n", argv[0]);
}
