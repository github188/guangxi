/*************************************
 *3GÁ÷Á¿¼à¿Ø
 *date:     2014.12.09
 *changelog:
 *
 *************************************/

#ifndef __FLOW_MONITOR_H__
#define __FLOW_MONITOR_H__
#include "sqlite3.h"

int flow_monitor_handle_thread_start();
void *flow_monitor_handle_thread(void *s);
int flow_monitor_get_current_value(char *net, int *rx, int *tx);
int flow_monitor_get_value_from_database(sqlite3 *db, char *type, int year, int month);
int flow_monitor_update_value_in_database(sqlite3 *db, char *type, int year, int month, int value);
int flow_monitor_insert_new_record_to_database(sqlite3 *db, int year, int month, int total);
int	get_flow_value_callback(void *para, int column, char **columnValue, char **columnName);
void *order_handle_get_flow_value_req_thread(void *s);

int flow_net_interface_judge(char *interface);

#endif