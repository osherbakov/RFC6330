#ifndef __RFC6330_H__
#define __RFC6330_H__

void encode_block(unsigned char *Result, unsigned int *ESIs, 
						  unsigned int NumSymbols,
						  unsigned char *Source,  unsigned int BytesPerSymbol,
						  unsigned int NumSrcBytes);

int decode_block(unsigned char *Result, unsigned int NumResultBytes,  
						  unsigned char *Source, unsigned int BytesPerSymbol, 
						  unsigned int *ESIs, unsigned int NumSymbols);

#endif
