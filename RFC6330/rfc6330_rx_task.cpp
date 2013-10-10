#include "rfc6330_tasks.h"

static WORKING_AREA(waRx, 200);
static bool	isActive = false;
static int	currSlot = 0;
static int	currChannel = 0;
static int  currSymbol = 0;
static int	*ESI;
static int	*currESI;
static uint8_t *currData;

static uint8_t tx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13
static uint8_t rx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13

msg_t rx_task(void *arg) {
	int			i;
	uint8_t		status;
	systime_t	t =  chTimeNow();
	while (1) 
	{
		t += MS2ST(1);
		chThdSleepUntil(t);

		if (isActive)
		{
			bool tx_ok, tx_fail, rx_rdy;

			radio.ce(LOW);
			radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
			radio.setChannel(currChannel);
			radio.whatHappened(tx_ok, tx_fail, rx_rdy);
			if(rx_rdy)
			{
				// Get ISI as the first byte of the packet
				radio.csn(LOW);
				SPI.transfer(R_RX_PAYLOAD);
				*currESI++ = SPI.transfer(0xFF);
				// Get the rest of data
				for(i = 0; i < bytes_per_symbol; i++ ){
					*currData++ = SPI.transfer(0xFF);
				}
				radio.csn(HIGH);
				currSymbol++;
			}

			if( currSymbol < num_symbols)
			{
				// Set Rx mode
				radio.write_register(CONFIG, radio.read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX) );
				radio.flush_rx();
				// Activate radio
				radio.ce(HIGH);
			}else if(currSlot >= num_timeslots)
			{
				isActive = false;
			}else // We already received all symbols we need - send ACK
			{
				radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
				radio.flush_tx();
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
			currSlot++;
		}
		// Keep the channel running (hopping)
		currChannel++; currChannel &= 0x7F;
	}
}
//------------------------------------------------------------------------------

void rx_task_setup() {
	// start handler task
	chThdCreateStatic(waRx, sizeof(waRx), HIGHPRIO, rx_task, NULL);
	currChannel = 0;
	currSlot = 0;
	isActive = false;
	{
		radio.powerUp();
		radio.begin();
		radio.stopListening();
		radio.setAutoAck(false);
		radio.setChannel(currChannel);
		radio.setPALevel(RF24_PA_HIGH);
		radio.setCRCLength(RF24_CRC_16);
		radio.setDataRate(RF24_1MBPS);
		radio.setRetries(0,0);
		radio.setPayloadSize(packet_size);
		radio.write_register(SETUP_AW, 0x01);	// 3 bytes address
		radio.write_register(TX_ADDR, tx_addr, 3);
		radio.write_register(RX_ADDR_P0, rx_addr, 3);
		radio.write_register(RX_PW_P0, packet_size);
		radio.write_register(EN_RXADDR, ERX_P0);
		radio.write_register(DYNPD,0);
		radio.write_register(FEATURE, 0);
		radio.powerDown();
		radio.powerUp();
	}
}

void rx_task_start(int Channel, uint8_t *pData, int *pESI)
{
	currChannel = Channel;
	currSymbol = 0;
	currSlot = 0;
	ESI = currESI = pESI;
	currData = pData;
	isActive = true;
}

void rx_task_stop()
{
	isActive = false;
}
