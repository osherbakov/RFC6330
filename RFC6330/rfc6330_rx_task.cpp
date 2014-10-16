#include "rfc6330_tasks.h"

static WORKING_AREA(waRx, 200);

static int	currSlot = 0;
static int  currSymbol = 0;

// Initial and current pointer to the ISI and data
static unsigned int *ESI;
static uint8_t	*Data;
static unsigned int *currESI;
static uint8_t *currData;

// Addresses to listen to and to send back the ack...
static uint8_t rx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13
static uint8_t ack_addr[] = {0x06, 0x50, 0xED};	// Barker 13 and Barker 11

static int rx_payload()
{
	int		i;
	int		ISI;

	radio.csn(LOW);
	SPI.transfer(R_RX_PAYLOAD);
	// Get ISI as the first byte of the packet
	ISI = SPI.transfer(0xFF);
	*currESI++ = ISI;
	// Get the rest of data
	for(i = 0; i < bytes_per_symbol; i++ ){
		*currData++ = SPI.transfer(0xFF);
	}
	radio.csn(HIGH);
	currSymbol++;
	return ISI;
}

static void tx_ack()
{
	int i;
	radio.ce(LOW);
	radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
	radio.setChannel(get_channel());
	radio.flush_tx();
	radio.flush_rx();
	//------------------------ Send the Payload ------------
	radio.csn(LOW);
	SPI.transfer(W_TX_PAYLOAD);
	// Send received ISIs as a payload of the packet
	for(i = 0; i < (num_symbols + 1); i++ ){
		SPI.transfer(ESI[i]);
	}
	radio.csn(HIGH);
	//----------------- Activate radio --------------
	radio.ce(HIGH);
}

msg_t rx_task(void *arg) {
	int			ISI;
	int			rxSlot;
	systime_t	t =  chTimeNow();

	while (1) 
	{
		t += MS2ST(1);
		chThdSleepUntil(t);

		currSlot++;
		switch(rxtx_state)
		{
		case RX_LISTEN:
			if(radio.get_status() & _BV(RX_DR))
			{
				do{
					radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
					ISI = rx_payload();
					Serial.print(ISI);
				} while( !(radio.read_register(FIFO_STATUS) &  _BV(RX_EMPTY)) );

				// Calculate the timeslot based on received ISI
				if(ISI < num_symbols)
					rxSlot = ISI + 1;	// We received that ISI in a previous slot
				else
					rxSlot = 2 * ISI - num_symbols + 2;
				currSlot = rxSlot;
				if((currSymbol >= num_symbols) && (currSlot < num_timeslots))
				{
					tx_ack();
					rxtx_state = RX_ACK;
				}
			}
			break;

		case RX_ACK:
			rxtx_state = RX_SKIP;	// Skip the rest of the slots
			Serial.println("Rx:A");
			break;
		}

		if (isActive)
		{
			int	rx_rdy;
			rx_rdy = radio.get_status() & _BV(RX_DR);
			if(rx_rdy)
			{
				do{
					radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
					ISI = rx_payload();
					Serial.print(ISI);
				} while( !(radio.read_register(FIFO_STATUS) &  _BV(RX_EMPTY)) );
				
				// Calculate the timeslot
				if(ISI < num_symbols)
					currSlot = ISI + 1;
				else
					currSlot = 2 * ISI - num_symbols + 2;
			}

			if( currSymbol < num_symbols)
			{
				// Keep collecting packets - set Rx mode/
				Serial.println("Rx:L");
				radio.write_register(CONFIG, radio.read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX) );
				radio.setChannel(get_channel());
				// Activate radio
				radio.ce(HIGH);
			}else if(currSlot >= num_timeslots)
			{
				Serial.println("Rx:U");
				currData = Data;
				currESI = ESI;
				currSymbol = 0;
			}else if(rx_rdy)// We already received all the symbols we need - send ACK
			{
				tx_ack();

				Serial.println("Rx:A");
			}else	// This code will execute after ACK was sent
			{
				Serial.println("Rx:P");
				isActive = false;
				// Start processing
			}
			currSlot++;
		}
		// Keep the channel running (hopping)
		next_channel();
	}
}
//------------------------------------------------------------------------------

void rx_task_setup() {
	chThdCreateStatic(waRx, sizeof(waRx), HIGHPRIO, rx_task, NULL);
	isActive = false;
	radio_setup(ack_addr, rx_addr);
}

void rx_task_start(int Channel, uint8_t *pData, unsigned int *pESI)
{
	set_channel(Channel);
	currSymbol = 0;
	currSlot = 0;
	ESI = currESI = pESI;
	Data = currData = pData;
	isActive = true;
}

void rx_task_stop()
{
	isActive = false;
}
