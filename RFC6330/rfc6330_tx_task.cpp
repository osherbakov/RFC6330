#include "rfc6330_tasks.h"

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

static WORKING_AREA(waTx, 200);
static bool	isActive = false;
static int	currSlot = 0;
static int	currChannel = 0;
static int	*currESI;
static uint8_t *currData;

static uint8_t tx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13
static uint8_t rx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13

msg_t tx_task(void *arg) {
	uint8_t		status;
	int			i;
	systime_t	t =  chTimeNow();
	while (1) 
	{
		t += MS2ST(1);
		chThdSleepUntil(t);

		if (isActive)
		{
			// First send the systemic symbols on every timeslot
			// after that listen for the ack and then send repair symbol 
			if( currSlot < num_symbols)
			{
				// Set Tx mode
				radio.ce(LOW);
				radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
				radio.setChannel(currChannel);
				radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
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
			}else // We sent all systemic symbols, so now listen for ACK and send repair symbols
			{
				radio.ce(LOW);
				radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
				radio.setChannel(currChannel);

				if( (currSlot & 0x01) == 0)	// That is a listening slot
				{
					// Set Rx mode
					radio.write_register(CONFIG, radio.read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX) );
					radio.flush_rx();
					radio.flush_tx();
					// Activate radio
					radio.ce(HIGH);
				}else	//Tx slot, but we have to check for the ACK received
				{
					bool tx_ok, tx_fail, rx_rdy;
					radio.whatHappened(tx_ok, tx_fail, rx_rdy);
					if(rx_rdy)  // ACK received
					{
						isActive = false;
					}else
					{
						radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
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
						// Activate radio
						radio.ce(HIGH);
					}
				}
			}
			currSlot++;
		}
		// Keep the channel running (hopping)
		currChannel++; currChannel &= 0x7F;
	}
}
//------------------------------------------------------------------------------

void tx_task_setup() {
  // start handler task
  chThdCreateStatic(waTx, sizeof(waTx), HIGHPRIO, tx_task, NULL);

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

void tx_task_start(uint8_t *pData, int *pESI)
{
 if(isActive)  Serial.println("Tx overrun");

  currSlot = 0;
  currESI = pESI;
  currData = pData;
  isActive = true;
}
//------------------------------------------------------------------------------
