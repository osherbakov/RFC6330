#include "rfc6330_tasks.h"


static WORKING_AREA(waTx, 200);
static bool	isActive = false;
static int	currSlot = 0;
static int	currChannel = 0;
static unsigned int *currESI;
static uint8_t *currData;

static uint8_t tx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13
static uint8_t ack_addr[] = {0x06, 0x50, 0xED};	// Barker 13 and Barker 11

static void tx_payload()
{
	int			i;
	// Set Tx mode
	radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
	radio.setChannel(currChannel);
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
	systime_t	t =  chTimeNow();
	while (1) 
	{
		t += MS2ST(1);
		chThdSleepUntil(t);

		if (isActive)
		{
			radio.ce(LOW);

			// First send the systemic symbols on every timeslot
			// after that listen for the ack and then send repair symbol 
			if( currSlot < num_symbols)
			{
				Serial.println("Tx:S");
				tx_payload();
			}else if(currSlot >= num_timeslots)
			{
				Serial.println("Tx:U");
				isActive = false;
			}else // We already sent all systemic symbols, so now listen for ACK and send repair symbols
			{
				if( (currSlot & 0x01) == 0)	// That is a Rx slot
				{
					Serial.println("Tx:L");

					// Set Rx mode
					radio.write_register(CONFIG, radio.read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX) );
					radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
					radio.setChannel(currChannel);
					radio.flush_rx();
					// Activate radio
					radio.ce(HIGH);
				}else	//Tx slot, but we have to check for the ACK received
				{
					bool rx_rdy;
					rx_rdy = radio.get_status() & _BV(RX_DR);
					if(rx_rdy)  // ACK received
					{
						Serial.println("Tx:A");
						isActive = false;
					}else
					{
						Serial.println("Tx:R");
						tx_payload();
					}
				}
			}
			currSlot++;
		}
		// Keep the channel running (hopping)
//		currChannel++; currChannel &= 0x7F;
	}
}
//------------------------------------------------------------------------------

void tx_task_setup() {
	chThdCreateStatic(waTx, sizeof(waTx), HIGHPRIO, tx_task, NULL);
	isActive = false;
	radio_setup(tx_addr, ack_addr);
}

void tx_task_start(int Channel, uint8_t *pData, unsigned int *pESI)
{
	if(isActive)  Serial.println("Tx overrun");

	currChannel = Channel;
	currSlot = 0;
	currData = pData;
	currESI = pESI;
	isActive = true;
}

void tx_task_stop()
{
	isActive = false;
}

//------------------------------------------------------------------------------
