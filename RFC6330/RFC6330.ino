#include "rfc6330_func.h"
#include "Streaming.h"

#define num_symbols (11)
#define num_generated_symbols (50 )
#define bytes_per_symbol  (1)
#define source_bytes (num_symbols * bytes_per_symbol)


unsigned char Source[source_bytes];
unsigned char Encoded[num_generated_symbols * bytes_per_symbol];
unsigned char Received[num_generated_symbols * bytes_per_symbol];
unsigned char Dest[source_bytes];
unsigned int ESIs[num_generated_symbols];




/*----------- */
unsigned int heap[2500];
void *p_heap;
void *halloc(unsigned int mem_size)
{
  void *ret = p_heap;
  p_heap = (void *) ( (unsigned int) p_heap + mem_size);
  return ret;
}

void hfree(void *p_mem)
{
  p_heap = p_mem;
}
/*************/

void setup()
{
  int  ret;

  uint32_t time_start, time_end;
  p_heap = &heap[0];

  float erasure = 0.3;

  Serial.begin(115200);

  delay(5000);

  Serial << "*******Starting********" << endl;
  
  Serial << "p_heap = 0x" << _HEX((int) p_heap) <<  endl;

  // Populate the Source with data from 1 to 100
  for(int i = 0; i < source_bytes; i++) Source[i] = i + 1;

  while(1)
  {

    rfc6330_encode_block(Encoded, ESIs, num_generated_symbols, Source, bytes_per_symbol, source_bytes);
    time_start = micros();

    // Simulate the erasure channel
    int rcvd_idx = 0;
    for(int i = 0; i < num_generated_symbols; i++)
    {
      if( (rand() % 100) >= (int)(erasure * 100))
      {
        memcpy(Received + rcvd_idx * bytes_per_symbol, Encoded + i * bytes_per_symbol, bytes_per_symbol); 
        ESIs[rcvd_idx++] = i;
       
        if(rcvd_idx >= num_symbols)
        {
          ret = rfc6330_decode_block(Dest, source_bytes, Received, bytes_per_symbol, ESIs, rcvd_idx);
          if( ret == 0)
          {
            // decoded
            time_end = micros();
            Serial << "Received " << rcvd_idx << " symbols:";
            for(int k = 0; k < rcvd_idx; k++)    Serial << ESIs[k] << " ";  
            Serial << "Time Elapsed = " << (time_end - time_start) << " us" << endl;
            Serial << "Compare Src and DST = " << memcmp(Source, Dest, source_bytes) << endl;
            break;
          }else
          {
            Serial << "****************** " << i << "  " << rcvd_idx << " *********" << endl;
          }
        }

      }
    }

  }

}

void loop()
{


}


