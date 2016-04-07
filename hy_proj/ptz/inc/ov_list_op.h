/**************************************
 *preset与tour链表操作函数声明
 *date:     2014.6.27
 *changelog:
 *
 *************************************/

#ifndef __OV_LIST_OP__H__
#define __OV_LIST_OP__H__

#include "ov_list.h"
#include "ov_types.h"

void ov_add_preset_to_dev(ov_dev_t *dev, ov_preset_t *preset);
void ov_add_tour_to_dev(ov_dev_t *dev, ov_tour_t *tour);
void ov_remove_preset_from_list(ov_preset_t *preset);
void ov_show_all_presets_from_dev(ov_dev_t *dev);

void ov_add_tour_to_dev(ov_dev_t * dev,ov_tour_t * tour);
void ov_show_all_tours_from_dev(ov_dev_t * dev);

#endif

