#include <stdio.h>
#include "hy_zlog.h"

zlog_category_t *zc;

int init_zlog()
{
    int rc;


    rc = zlog_init("zlog.conf");
    if (rc)
    {
        printf("init failed\n");
        return -1;
    }

    zc = zlog_get_category("Jimmy_cat");
    if (!zc)
    {
        printf("get cat fail\n");
        zlog_fini();
        return -2;
    }

    return 0;
}

void fini_zlog()
{
    zlog_fini();
}
