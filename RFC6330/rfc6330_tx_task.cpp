#include "rfc6330_tasks.h"
#include <SPI.h>
#include <RF24.h>

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

//------------------------------------------------------------------------------
// handler task for interrupt
static WORKING_AREA(waTx, 200);
static int	currPacket = 0;
static int	currChannel = 0;

static uint8_t tx_addr[] = {0x1D, 0xA0, 0xCA};	// Barker 11 and Barker 13
static uint8_t test_data[] = {1,2,3,4,5,6,7,8,9,10};

msg_t tx_task(void *arg) {
 systime_t t =  chTimeNow();
  while (1) {
    t += MS2ST(1);
	chThdSleepUntil(t);

	{
		bool tx_ok, tx_fail, rx_rdy;
		radio.whatHappened(tx_ok, tx_fail, rx_rdy);
		radio.setChannel(currChannel);
		currChannel++; currChannel &= 0x7F;

		radio.write_register(CONFIG, ( radio.read_register(CONFIG) | _BV(PWR_UP) ) & ~_BV(PRIM_RX) );
		radio.write_payload(test_data, 10);
		radio.ce(HIGH);
		delayMicroseconds(15);
		radio.ce(LOW);
	} 

  }
}
//------------------------------------------------------------------------------

void tx_task_setup() {
  // start handler task
  chThdCreateStatic(waTx, sizeof(waTx), HIGHPRIO, tx_task, NULL);

  currChannel = 0;
  currPacket = 0;

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
	  radio.setPayloadSize(10);
	  radio.write_register(SETUP_AW, 0x01);	// 3 bytes address
	  radio.write_register(TX_ADDR, tx_addr, 3);
	  radio.write_register(RX_ADDR_P0, tx_addr, 3);
	  radio.write_register(RX_PW_P0, 10);
	  radio.write_register(DYNPD,0);
	  radio.write_register(FEATURE, 0);
	  radio.powerDown();
	  radio.powerUp();
  }
}
//------------------------------------------------------------------------------
