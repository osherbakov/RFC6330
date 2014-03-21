#ifndef __CVSD_H__
#define __CVSD_H__
#include "stdint.h"


#define BITRATE (16000)

#define BITRATE_KB (BITRATE / 1000)

#define SR_NUM_BITS			(3)
#define SR_MASK				((1<<SR_NUM_BITS)-1)

#define DATA_NUM_BITS		(16)

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


#define MAX_DATA			( (1L <<(DATA_NUM_BITS-1)) - 1)
#define MIN_DATA			( -(MAX_DATA + 1) )

#define DATA_NORM_SHIFT		(DATA_NUM_BITS - 1)
#define DATA_NORM_OFFSET	( 1L << (DATA_NORM_SHIFT - 1) )
#define DATA_NORM_DIV		( 1L << DATA_NORM_SHIFT )


#define RC_SYLLABIC_STEP_MS		(5)
#define RC_SYLLABIC_LEAK_MS		(5)
#define RC_INTEGRATOR_STEP_MS	(1)
#define RC_INTEGRATOR_LEAK_MS	(1)

#define MAX_SLOPE			(16)
#define SYLLABIC_RATIO		(32 * MAX_SLOPE)

#define DIV(a,b)  (uint16_t)(((a)+(b)/2)/(b))

// static __inline int16_t FP_MULT(int32_t a, uint16_t b) { return (int16_t) ((a * b + DATA_NORM_OFFSET) >> DATA_NORM_SHIFT);}
static __inline int16_t FP_MULT(int32_t a, uint16_t b) { return (int16_t) MAX(MIN(((float)a * (float)b)/(float)DATA_NORM_DIV, (float)MAX_DATA), (float)MIN_DATA);}

#define	SYLLABIC_STEP	DIV(MAX_DATA, BITRATE_KB * RC_SYLLABIC_STEP_MS)
#define	SYLLABIC_LEAK	DIV(MAX_DATA, BITRATE_KB * RC_SYLLABIC_LEAK_MS)

#define	SYLLABIC_MIN	DIV(MAX_DATA, SYLLABIC_RATIO)

#define	INTEGRATOR_STEP	DIV(MAX_DATA, BITRATE_KB * RC_INTEGRATOR_STEP_MS)
#define	INTEGRATOR_LEAK	DIV(MAX_DATA, BITRATE_KB * RC_INTEGRATOR_LEAK_MS)

#define N_COEFF		(5)

typedef struct CVSD_STATE{
	uint8_t		ShiftRegister;
	uint32_t	V_syllabic;
	int32_t		V_integrator;
	uint32_t	n_coeff;
	int16_t		filt_den[N_COEFF];
	int16_t		filt_num[N_COEFF];
	int16_t		filt_states[N_COEFF-1];
}CVSD_STATE_t;

const int16_t FILT_NUM[] = {
// 851,   2221,   3002,   2221,    851
      498,   1990,   2985,   1990,    498
};

const int16_t FILT_DEN[] = {
//    16384, -21824,  25281, -14327,   4758
    16384, -22800,  25331, -14678,   4693
};


typedef enum
{
	CVSD_ENC,
	CVSD_DEC
}CVSD_TYPE_t;

extern void cvsd_init(CVSD_STATE_t *state, CVSD_TYPE_t cvsd_type);
extern void cvsd_encode_buffer(CVSD_STATE_t *state, uint8_t *pBits, int16_t *pSamples, int nSamples);
extern void cvsd_decode_buffer(CVSD_STATE_t *state, int16_t *pSamples, uint8_t *pBits, int nBits);
extern int cvsd_filter(CVSD_STATE_t *state, int sample);

extern void cvsd_8K_to_16K(CVSD_STATE_t *state, int16_t *dest, int16_t *src, int src_cnt);
extern void cvsd_16K_to_8K(CVSD_STATE_t *state, int16_t *dest, int16_t *src, int src_cnt);

extern void fill_test_data(uint8_t *pData, int insert_offset, int run_size_bytes, int num_bytes);

#endif	// __CVSD_H__

