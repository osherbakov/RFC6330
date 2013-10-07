#include "rfc6330_func.h"
#include "rfc6330_tasks.h"
#include "Streaming.h"
#include <ChibiOS_ARM.h>

#define erasure (0.6)

// The LED is attached to pin 13 on Arduino and Teensy 3.0, pin 6 on Teensy++ 2.0.
const uint8_t LED_PIN = 13;

unsigned char Source[source_bytes];
unsigned char Encoded[encoded_bytes];
uint16_t ESIs[num_generated_symbols];
unsigned char Received[num_generated_symbols * bytes_per_symbol];
unsigned char Dest[source_bytes];


void setup()
{
  Serial.begin(115200);
  while(!Serial) {}

  // Start ChibiOS
  chBegin(mainThread);
  while (1) {}
}

rfc6330_state_t enc_state;
rfc6330_state_t dec_state;

void mainThread()
{
  int  ret;
  uint32_t time_start, time_end;
  
  // Start handler task
  task_sample_setup();
  task_rf_send_setup();

  Serial << "*******Starting********" << endl;
  
//  Serial << "p_heap = 0x" << _HEX((int) p_heap) <<  endl;

  // Populate the Source with data from 1 to 100
  for(int i = 0; i < source_bytes; i++) Source[i] = i + 1;

  while(1)
  {

    rfc6330_encode_block(&enc_state, Encoded, ESIs, num_generated_symbols, Source, bytes_per_symbol, source_bytes);
    time_start = micros();

    // cause an interrupt - normally done by external event
    Serial.println("High");
    digitalWrite(OUTPUT_PIN, HIGH);
    Serial.println("Low");
    digitalWrite(OUTPUT_PIN, LOW);
    Serial.println();

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
          ret = rfc6330_decode_block(&dec_state, Dest, source_bytes, Received, bytes_per_symbol, ESIs, rcvd_idx);
          if( ret == 0)
          {
            // decoded succesfully - print results
            time_end = micros();
            Serial << "Received " << rcvd_idx << " symbols:";
            for(int k = 0; k < rcvd_idx; k++)    Serial << ESIs[k] << " ";  
            Serial << "Time Elapsed = " << (time_end - time_start) << " us" << endl;
            Serial << "Compare Src and DST = " << memcmp(Source, Dest, source_bytes) << endl;
            break;
          }else
          {
            Serial << "****************** Extra Symbol needed *********" << endl;
          }
        }

      }
    }

  }

}

void loop()
{


}


