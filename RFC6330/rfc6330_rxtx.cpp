#include "rfc6330_tasks.h"

void radio_setup(uint8_t *tx_addr, uint8_t *rx_addr)
{
	radio.begin();
	radio.ce(LOW);
	radio.write_register(CONFIG,  _BV(MASK_TX_DS) |  _BV(MASK_MAX_RT) );

	radio.powerUp();
	chThdSleepMilliseconds(5);
	radio.stopListening();
	radio.write_register(STATUS, _BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
	radio.setAutoAck(false);
	radio.setPALevel(RF24_PA_HIGH);
	radio.setCRCLength(RF24_CRC_16);
	radio.setDataRate(RF24_250KBPS);
	radio.setRetries(0,0);
	radio.setPayloadSize(packet_size);
	radio.write_register(SETUP_AW, 0x01);	// 3 bytes address
	radio.write_register(TX_ADDR, tx_addr, 3);
	radio.write_register(RX_ADDR_P0, rx_addr, 3);
	radio.write_register(RX_PW_P0, packet_size);
	radio.write_register(EN_RXADDR, _BV(ERX_P0));
	radio.write_register(DYNPD,0);
	radio.write_register(FEATURE, 0);
}

void rx_mode()
{
	// Set Rx mode
   	radio.ce(LOW);
	radio.write_register(CONFIG, radio.read_register(CONFIG) | _BV(PWR_UP) | _BV(PRIM_RX) );
	radio.write_register(STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );
	radio.setChannel(get_channel());
    radio.flush_tx();
    radio.flush_rx();
	// Activate radio
	radio.ce(HIGH);
}

static int 	currChannel;
void set_channel(int channel)
{
	currChannel = channel;
}
int get_channel()
{
	return currChannel;
}
void next_channel()
{
	currChannel++; currChannel &= 0x7F;
}
