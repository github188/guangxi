#ifndef _CHECK_H_
#define _CHECK_H_

#include <sys/types.h>

u_int16_t CRC16( u_int8_t *PuchMsg,u_int16_t usDataLen);
u_int8_t checksum(u_int8_t *pdata,u_int16_t length);
void CRC16_init(u_int16_t *crc16);
void CRC16_update(u_int16_t *crc16, u_int8_t *PuchMsg,u_int32_t usDataLen);
u_int8_t check_xor(u_int8_t *pdata,u_int16_t length);
#endif
