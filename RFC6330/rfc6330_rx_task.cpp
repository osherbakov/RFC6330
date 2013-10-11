#include "rfc6330_tasks.h"

static WORKING_AREA(waRx, 200);
static bool	isActive = false;
static int	currSlot = 0;
static int	currChannel = 0;
static int  currSymbol = 0;
static unsigned int *ESI;
static uint8_t	*Data;
static unsigned int *currESI;
static uint8_t *currData;

static uint8_t rx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13
static uint8_t ack_addr[] = {0x06, 0x50, 0xED};	// Barker 13 and Barker 11


msg_t rx_task(void *arg) {
	int			i;
	uint8_t		status;
	uint8_t		ISI;
	systime_t	t =  chTimeNow();
	bool		rx_rdy;

	while (1) 
	{
		t += MS2ST(1);
		chThdSleepUntil(t);

		if (isActive)
		{
			rx_rdy = radio.get_status() & _BV(RX_DR);
			if(rx_rdy)
			{
				Serial.print("Rx:");
				do
				{
					// Get ISI as the first byte of the packet
					radio.csn(LOW);
					SPI.transfer(R_RX_PAYLOAD);
					ISI = SPI.transfer(0xFF);
					*currESI++ = ISI;
					Serial.println(ISI);
					// Get the rest of data
					for(i = 0; i < bytes_per_symbol; i++ ){
						*currData++ = SPI.transfer(0xFF);
					}
					radio.csn(HIGH);
					currSymbol++;
					radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
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
				radio.setChannel(currChannel);
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
				Serial.println("Rx:A");
				radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
				radio.setChannel(currChannel);
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
			}else	// This code will execute after ACK was sent
			{
				Serial.println("Rx:P");
				isActive = false;
				// Start processing
			}
			currSlot++;
		}
		// Keep the channel running (hopping)
//		currChannel++; currChannel &= 0x7F;
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
	currChannel = Channel;
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
