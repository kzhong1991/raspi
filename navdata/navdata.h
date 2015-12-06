/*****************************************************************************
 * @file    navData.h
 * @author  kzhong1991<kzhong1991@gmail.com>
 * @date    2015-12-03
 * @brief   This thread is used to receive navigation data send by ardrone
 *****************************************************************************/
#ifndef NVADATA_H
#define NVADATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/ardrone.h"
#include "../common/config.h"
#include "navdata_common.h"
#include "navdata_keys.h"

#define MAX_NAVDATA_SIZE (4096)

#define ardrone_navdata_unpack(navdata_ptr, option) (navdata_option_t*) navdata_unpack_option((uint8_t*) navdata_ptr, \
                                                                                                navdata_ptr->size,    \
                                                                                                (uint8_t*) &option,   \
                                                                                                sizeof (option))

void navdata_init();
void nav_thread_exit();


#endif
