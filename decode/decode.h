#ifndef _HY_DECODE_H
#define _HY_DECODE_H

#include "hy_type.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void *hy_decode_thread_cb(void *s);
void *hy_start_decode_thread();
void *hy_stop_decode_thread();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
