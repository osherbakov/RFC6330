#ifndef RFC6330_TASKS_H
#define RFC6330_TASKS_H

#include <Arduino.h>
#include <ChibiOS_ARM.h>
#include <stdint.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#define num_symbols (10)			// We will send 100 bytes every 50 ms
#define bytes_per_symbol  (10)		//    split into 10 packets of 10 bytes each
#define num_timeslots		(50)	// 50 timeslots 1 ms each
#define num_generated_symbols (30)	// In total we will send 30 packets max,
									//  10 original (10ms) + 20 extra (40ms)
#define source_bytes	(num_symbols * bytes_per_symbol)
#define encoded_bytes	(num_symbols * bytes_per_symbol)
#define packet_size		(bytes_per_symbol + 1)	// Send ISI + data

//------------------------------------------------------------------------------------
// Common functions and parameters for both Rx and Tx tasks
extern RF24 radio;
extern void radio_setup(uint8_t *tx_addr, uint8_t *rx_addr);
extern void rx_mode();

extern void set_channel(int channel);
extern int get_channel();
extern void next_channel();

typedef enum 
{
	RXTX_IDLE = 0,
	TX_IDLE,
	TX_SYSTEMIC,
	TX_DONE,
	TX_LISTEN,
	TX_REPAIR,

	RX_IDLE,
	RX_LISTEN,
	RX_ACK,
	RX_SKIP,
	RX_FAIL,


}RXTX_STATES;

extern RXTX_STATES rxtx_state;

//------------------------------------------------------------------------------------
// Parameters for TX task
extern void tx_task_setup();
extern void tx_task_start(int Channel, uint8_t *pData, unsigned int *pESI);
extern void tx_task_stop();
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Parameters for RX task
extern void rx_task_setup();
extern void rx_task_start(int Channel, uint8_t *pData, unsigned int *pESI);
extern void rx_task_stop();
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Parameters for ISR task that will will be activated on IRQ
// pin to generate interrupts - this pin is connected to a IRQ pin of nRF24L01+
const uint8_t IRQ_INPUT_PIN = 2;
extern void isr_task_setup();
//------------------------------------------------------------------------------------

#endif
