#include "rfc6330_tasks.h"


WORKING_AREA(waTx, 200);
static int	currSlot = 0;
static unsigned int *currESI;
static uint8_t *currData;

static uint8_t tx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13
static uint8_t ack_addr[] = {0x06, 0x50, 0xED};	// Barker 13 and Barker 11

static void tx_payload()
{
	int			i;
	// Set Tx mode
	radio.ce(LOW);
	radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
	radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
	radio.setChannel(get_channel());
    radio.flush_tx();
    radio.flush_rx();
	//------------------------ Send the Payload ------------
	// Send ISI as the first byte of the packet
	radio.csn(LOW);
	SPI.transfer(W_TX_PAYLOAD);
	SPI.transfer(*currESI++);
	// Send the rest of data
	for(i = 0; i < bytes_per_symbol; i++ ){
		SPI.transfer(*currData++);
	}
	radio.csn(HIGH);
	//----------------- Activate radio --------------
	radio.ce(HIGH);
}


msg_t tx_task(void *arg) 
{
	bool rx_rdy;
	systime_t	t =  chTimeNow();
	while (1) 
	{
		t += MS2ST(1);
		chThdSleepUntil(t);

		switch(rxtx_state)
		{
		case TX_SYSTEMIC:	// We are sending systemics
			tx_payload();
			currSlot++;
			if(currSlot >= num_symbols)	// Start listening to the ACK
			{
				rxtx_state = TX_REPAIR;
			}
			if(currSlot >= num_timeslots)
			{
				rxtx_state = TX_IDLE;
			}
			break;

		case TX_DONE:		// Tx was done - keep listening
			rxtx_state = TX_LISTEN;
			currSlot++;
			if(currSlot >= num_timeslots)
			{
				rxtx_state = TX_IDLE;
			}
			break;

		case TX_LISTEN:		// Listen slot ended - check and send next
			rx_rdy = radio.get_status() & _BV(RX_DR);
			if(rx_rdy)  // ACK received
			{
       	        radio.ce(LOW);
                radio.flush_rx();
				rxtx_state = TX_IDLE;
				Serial.println("Tx:ACK");
			}else
			{
				tx_payload();
				rxtx_state = TX_REPAIR;
			}
			currSlot++;
			if(currSlot >= num_timeslots)
			{
				rxtx_state = TX_IDLE;
			}
			break;

		default:
			break;
		}
		// Keep the channel running (hopping)
		next_channel();
	}
}


//------------------------------------------------------------------------------
void tx_task_setup() {
	chThdCreateStatic(waTx, sizeof(waTx), HIGHPRIO, tx_task, NULL);
	rxtx_state = TX_IDLE;
	radio_setup(tx_addr, tx_addr);
}

void tx_task_start(int Channel, uint8_t *pData, unsigned int *pESI)
{
	set_channel(Channel);
	currSlot = 0;
	currData = pData;
	currESI = pESI;
	rxtx_state = TX_SYSTEMIC;
	Serial.println("Tx:Start");
}

void tx_task_stop()
{
    radio.ce(LOW);
    radio.flush_rx();
    radio.flush_tx();
	rxtx_state = TX_IDLE;
	rxtx_state = TX_IDLE;
	Serial.println("Tx:Stop");
}

//------------------------------------------------------------------------------
