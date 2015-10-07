#include "func.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NUM_SYMBOLS			(10)
#define GENERATE_SYMBOLS	(20)
#define BYTES_PER_SYMBOL	(1)

#define ERASURE_RATE		(0.5f)

#define SOURCE_BYTES		(NUM_SYMBOLS * BYTES_PER_SYMBOL)



unsigned char Source[SOURCE_BYTES]; // = {73, 110, 32, 116, 104, 101, 32, 98, 101, 103, 105 , 110, 110, /* 105, 110, 103*/};
unsigned char Dest[SOURCE_BYTES];

unsigned char Encoded[GENERATE_SYMBOLS * BYTES_PER_SYMBOL];
unsigned char Received[GENERATE_SYMBOLS * BYTES_PER_SYMBOL];
unsigned int ESIs[GENERATE_SYMBOLS];
unsigned int ERIs[GENERATE_SYMBOLS];





/*----------- */
unsigned int heap[2500];
void *p_heap;
void *p_heap_start;
void *p_heap_end;

void *osAlloc(unsigned int req_mem_size)
{
  void *ret = p_heap;
  void *new_heap_end = (void *)((unsigned int)p_heap + req_mem_size);
  if(p_heap_end < new_heap_end) 
	  return 0;
  p_heap = new_heap_end;
  return ret;
}

void osFree(void *p_mem)
{
  if((p_mem > p_heap) || (p_mem < p_heap_start)) return;
  p_heap = p_mem;
}
/*************/

void main()
{
  int  ret;

  int	numBlocks;
  int	numReceived;
  int	numDecoded;
  int	numRequredExtra;

  p_heap_start = &heap[0];
  p_heap_end = &heap[0] + (sizeof(heap) * sizeof(unsigned int));
  p_heap = p_heap_start;

  printf("*******Starting********\n");
  

  // Populate the Source with data from 1 to SOURCE_BYTES
  for(int i = 0; i < SOURCE_BYTES; i++) Source[i] = i + 1;

  numBlocks = numDecoded = numRequredExtra = numReceived = 0;
  while(1)
  {
	numBlocks++;
    encode_block(Encoded, ESIs, GENERATE_SYMBOLS, Source, BYTES_PER_SYMBOL, SOURCE_BYTES);

    // Simulate the erasure channel
    int rcvd_idx = 0;
    for(int i = 0; i < GENERATE_SYMBOLS; i++)
    {
      if( (rand() % 100) >= (int)(ERASURE_RATE * 100))
      {
		// We received the symbol OK thru the channel - put it into buffer and store the index
        memcpy(Received + rcvd_idx * BYTES_PER_SYMBOL, Encoded + i * BYTES_PER_SYMBOL, BYTES_PER_SYMBOL); 
        ERIs[rcvd_idx] = ESIs[i];
        rcvd_idx++;
        if(rcvd_idx >= NUM_SYMBOLS)
        {
		  if(rcvd_idx == NUM_SYMBOLS) numReceived++;
          ret = decode_block(Dest, SOURCE_BYTES, Received, BYTES_PER_SYMBOL, ERIs, rcvd_idx);
          if( ret != 0)
          {
			  numRequredExtra++;
			  continue;
		  }
		  if(memcmp(Source, Dest, SOURCE_BYTES) == 0){ 
			  numDecoded++;
			  break;
		  }
        }
      }
    }
	printf("Sent: %d\tRcvd: %d(%2.f%%)\tCorrect: %d(%2.f%%)\tExtra: %d\r", 
		numBlocks, numReceived, numReceived*100.f/numBlocks,
		numDecoded, numDecoded*100.f/numReceived, numRequredExtra);

  }

}


