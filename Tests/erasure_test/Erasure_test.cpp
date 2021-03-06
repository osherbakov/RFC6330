#include "RFC6330func.h"
#include "RFC6330.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NUM_SYMBOLS			(10)
#define GENERATE_SYMBOLS	(15)
#define BYTES_PER_SYMBOL	(1)

#define ERASURE_RATE		(0.4f)

#define SOURCE_BYTES		(NUM_SYMBOLS * BYTES_PER_SYMBOL)



unsigned char Source[SOURCE_BYTES]; // = {73, 110, 32, 116, 104, 101, 32, 98, 101, 103, 105 , 110, 110, /* 105, 110, 103*/};
unsigned char Dest[SOURCE_BYTES];

unsigned char Encoded[GENERATE_SYMBOLS * BYTES_PER_SYMBOL];
unsigned char Received[GENERATE_SYMBOLS * BYTES_PER_SYMBOL];
unsigned int ESIs[GENERATE_SYMBOLS];
unsigned int ERIs[GENERATE_SYMBOLS];



/*************/

void main()
{
  int  ret;

  int	numBlocks;
  int	numReceived;
  int	numDecoded;
  int	numRequredExtra;

  osHeapInit();

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


