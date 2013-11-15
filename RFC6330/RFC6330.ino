#include "rfc6330_func.h"
#include <ChibiOS_ARM.h>
#include <SPI.h>
#include <RF24.h>

#include "rfc6330_tasks.h"

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

#include "rfc6330_tasks.h"

#define erasure (0.0)
#define num_symbols (10)
#define num_generated_symbols (30)
#define bytes_per_symbol  (10)
#define source_bytes (num_symbols * bytes_per_symbol)

// The LED is attached to pin 13 on Arduino and Teensy 3.0, pin 6 on Teensy++ 2.0.
const uint8_t LED_PIN = 13;

unsigned char Source[source_bytes];
unsigned char Encoded[num_generated_symbols * bytes_per_symbol];
unsigned char Received[num_generated_symbols * bytes_per_symbol];
unsigned char Dest[source_bytes];
unsigned int ESIs[num_generated_symbols];

void setup()
{
  Serial.begin(9600);
  while(!Serial) {}

  Serial.println("*******Setup()********");

  // Start ChibiOS
  chBegin(mainThread);
  while (1) {}
}


void mainThread()
{
  int  ret;

  uint32_t time_start, time_end;

  Serial.println("*******Starting********");
  sample_task_setup();
  tx_task_setup();
//  rx_task_setup();
  
  // Populate the Source with data from 1 to 100
  for(int i = 0; i < source_bytes; i++) Source[i] = i + 1;
  rfc6330_encode_block(Encoded, ESIs, num_generated_symbols, Source, bytes_per_symbol, source_bytes);
  while(1)
  {
    chThdSleepMilliseconds(1000);

  Serial.println("*******Starting********");

    tx_task_start(1, Encoded, ESIs);
    
//	rx_task_start(22, Received, ESIs);
/***********************
	time_start = micros();

    // Simulate the erasure channel
    int rcvd_idx = 0;
    for(int i = 0; i < num_generated_symbols; i++)
    {
      if( (rand() % 100) >= (int)(erasure * 100))
      {
        memcpy(Received + rcvd_idx * bytes_per_symbol, Encoded + i * bytes_per_symbol, bytes_per_symbol); 
        ESIs[rcvd_idx++] = ESIs[i];
       
        if(rcvd_idx >= num_symbols)
        {
          ret = rfc6330_decode_block(Dest, source_bytes, Received, bytes_per_symbol, ESIs, rcvd_idx);
          if( ret == 0)
          {
            // decoded succesfully - print results
            time_end = micros();
            Serial << "Received " << rcvd_idx << " symbols:";
            for(int k = 0; k < rcvd_idx; k++)    Serial << ESIs[k] << " ";  
            Serial << "Time Elapsed = " << (time_end - time_start) << " us" << endl;
//            Serial << "Compare Src and DST = " << memcmp(Source, Dest, source_bytes) << endl;
            break;
          }else
          {
            Serial << "****************** Extra Symbol needed *********" << endl;
          }
        }
      }
    }
******************/
  }
}

void loop()
{
}


