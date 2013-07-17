#ifndef __CVSD_H__
#define __CVSD_H__
#include "stdint.h"

#define SAMPLERATE (8000)
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

#define SYLLABIC_RATIO		(33)


#define	SYLLABIC_STEP	( (MAX_DATA + (BITRATE_KB * RC_SYLLABIC_STEP_MS)/2 ) / ( BITRATE_KB * RC_SYLLABIC_STEP_MS))
#define	SYLLABIC_LEAK	( (MAX_DATA + (BITRATE_KB * RC_SYLLABIC_LEAK_MS)/2 ) / ( BITRATE_KB * RC_SYLLABIC_LEAK_MS))
#define	SYLLABIC_MIN	( (MAX_DATA + SYLLABIC_RATIO/2)/SYLLABIC_RATIO )

#define	INTEGRATOR_STEP		( (MAX_DATA + (BITRATE_KB * RC_INTEGRATOR_STEP_MS)/2 ) / ( BITRATE_KB * RC_INTEGRATOR_STEP_MS))
#define	INTEGRATOR_LEAK		( (MAX_DATA + (BITRATE_KB * RC_INTEGRATOR_LEAK_MS)/2 ) / ( BITRATE_KB * RC_INTEGRATOR_LEAK_MS))

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

#endif	// __CVSD_H__

