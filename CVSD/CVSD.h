#ifndef __CVSD_H__
#define __CVSD_H__
#include "stdint.h"

#define SAMPLERATE (16000)
#define BITRATE (16000)

#define SAMPLERATE_KS (SAMPLERATE / 1000)
#define BITRATE_KB (BITRATE / 1000)

#define SR_NUM_BITS			(3)
#define SR_MASK				((1<<SR_NUM_BITS)-1)

#define DATA_NUM_BITS		(16)

#define MAX_DATA			( (1L <<(DATA_NUM_BITS-1)) - 1)
#define MIN_DATA			( -(MAX_DATA + 1) )

#define DATA_NORM_SHIFT		(DATA_NUM_BITS - 1)
#define DATA_NORM_OFFSET	( 1L << (DATA_NORM_SHIFT - 1) )


#define RC_SYLLABIC_STEP_MS	(5)
#define RC_SYLLABIC_LEAK_MS	(5)
#define RC_INTEGRATOR_STEP_MS	(1)
#define RC_INTEGRATOR_LEAK_MS	(1)

#define MAX_SLOPE			(20)
#define SYLLABIC_RATIO		(48)

#define DIV(a,b)  (((a)+(b)/2)/(b))
#define MULT(a,b) (((a)*(b)+DATA_NORM_OFFSET)>>DATA_NORM_SHIFT)

#define	SYLLABIC_STEP	DIV(MAX_DATA, BITRATE_KB * RC_SYLLABIC_STEP_MS)
#define	SYLLABIC_LEAK	DIV(MAX_DATA, BITRATE_KB * RC_SYLLABIC_LEAK_MS)
#define	SYLLABIC_MIN	DIV(MAX_DATA, SYLLABIC_RATIO)

#define	INTEGRATOR_STEP	DIV(MAX_DATA, BITRATE_KB * RC_INTEGRATOR_STEP_MS)
#define	INTEGRATOR_LEAK	DIV(MAX_DATA, BITRATE_KB * RC_INTEGRATOR_LEAK_MS)

#define N_COEFF_DEC		(22)
#define N_STATES_DEC    (N_COEFF_DEC-1)

typedef struct CVSD_STATE{
	uint8_t		ShiftRegister;
	uint16_t	V_syllabic;
	int16_t		V_integrator;
	int16_t		dec_coeff[N_COEFF_DEC];
	int16_t		dec_states[N_STATES_DEC];
}CVSD_STATE_t;

;
const int16_t DEC_NUM[] = {
      730,    453,  -1079,  -1351,   1132,   2872,   -441,  -5424,  -2262,
    12092,  26377,  26377,  12092,  -2262,  -5424,   -441,   2872,   1132,
    -1351,  -1079,    453,    730
};

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


extern uint8_t cvsd_encode(CVSD_STATE_t *state, int sample);
extern int cvsd_decode(CVSD_STATE_t *state, uint8_t bits);
extern void cvsd_init(CVSD_STATE_t *state);
extern int cvsd_postfilter(CVSD_STATE_t *state, int sample);

#endif	// __CVSD_H__

