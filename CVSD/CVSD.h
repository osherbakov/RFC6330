#ifndef __CVSD_H__
#define __CVSD_H__
#include "stdint.h"

#define SAMPLERATE (8000)
#define BITRATE (16000)

#define SAMPLERATE_KS (SAMPLERATE / 1000)
#define BITRATE_KB (BITRATE / 1000)

// Shift register info
#define SR_NUM_BITS			(3)
#define SR_MASK				((1<<SR_NUM_BITS)-1)

// Input/output data info 
#define DATA_NUM_BITS		(13)
#define DATA_NORM_SHIFT		(DATA_NUM_BITS - 1)
#define DATA_NORM_OFFSET	( 1L << (DATA_NORM_SHIFT - 1) )

#define MAX_ABS_DATA		(1L << DATA_NORM_SHIFT)

#define RC_SYLLABIC_MS		(5)
#define SYLLABIC_RATIO		(33)
#define RC_INTEGRATOR_MS	(1)

#define	SYLLABIC_STEP		( (MAX_ABS_DATA + (BITRATE_KB * RC_SYLLABIC_MS)/2 ) / ( BITRATE_KB * RC_SYLLABIC_MS))
#define	SYLLABIC_MIN		( ((MAX_ABS_DATA + SYLLABIC_RATIO/2)/SYLLABIC_RATIO + (BITRATE_KB * RC_SYLLABIC_MS)/2 ) / ( BITRATE_KB * RC_SYLLABIC_MS))

#define	INTEGRATOR_STEP		( (MAX_ABS_DATA + (BITRATE_KB * RC_INTEGRATOR_MS)/2 ) / ( BITRATE_KB * RC_INTEGRATOR_MS))

typedef struct CVSD_STATE{
	uint8_t		ShiftRegister;
	uint16_t	V_syllabic;
	int16_t		V_integrator;
}CVSD_STATE_t;

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

extern int cvsd_decode(CVSD_STATE_t *state, uint8_t bits);
extern uint8_t cvsd_encode(CVSD_STATE_t *state, int sample);
extern void cvsd_init(CVSD_STATE_t *state);

#endif	// __CVSD_H__

