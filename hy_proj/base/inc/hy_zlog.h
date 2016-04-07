#ifndef _HY_ZLOG_H
#define _HY_ZLOG_H
#include "zlog.h"

#define hy_zlog_info(fmt...)\
    do{                                   \
        zlog_info(zc, fmt); \
    }while(0)

#define hy_zlog_debug(fmt...)\
    do{                                   \
        zlog_debug(zc, fmt); \
    }while(0)

extern zlog_category_t *zc;

int init_zlog();
void fini_zlog();

#endif

