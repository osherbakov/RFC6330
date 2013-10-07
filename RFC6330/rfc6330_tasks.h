#ifndef RFC6330_TASKS_H
#define RFC6330_TASKS_H

#include <Arduino.h>
#include <ChibiOS_ARM.h>
#include <stdint.h>

//------------------------------------------------------------------------------------
// Parameters for TX symbols
#define num_symbols (10)			// We will send 100 bytes every 50 ms
#define bytes_per_symbol  (10)		//    split into 10 packets of 10 bytes each
#define num_generated_symbols (30)	// In total we will send 30 packets max,
									//  10 original (10ms) + 20 extra (40ms)
#define source_bytes (num_symbols * bytes_per_symbol)
#define encoded_bytes (num_symbols * bytes_per_symbol)

extern unsigned char *pEnc;
extern unsigned char *pTx;

extern BinarySemaphore encSem;
extern BinarySemaphore txSem;

extern unsigned char Source[source_bytes];
extern unsigned char Encoded[encoded_bytes];
extern void task_rf_send_setup();
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Parameters for ISR sample task
// pins to generate interrupts - these pins must be connected with a wire
const uint8_t INPUT_PIN = 2;
const uint8_t OUTPUT_PIN = 3;
extern void task_sample_setup();
//------------------------------------------------------------------------------------

#endif
