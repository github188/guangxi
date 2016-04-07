#ifndef _DSPVIDEODECODEC_H_
#define _DSPVIDEODECODEC_H_
#include <stdio.h>

#include <ti/sdo/dmai/BufTab.h>

#define DECODE_TEST 0

typedef struct _BufInfo{
	unsigned char *ptr;
	size_t 	buf_size;
	size_t 	num_byteused;
} BufInfo;
void destroy();
int tidec_init(void);
int tidec_process(BufInfo *user_inbuf, BufInfo *user_outbuf, int *got_picture);
unsigned char * dsp_memory_alloc(int size);
void dsp_memory_free(unsigned char *ptr, int size);
#endif
