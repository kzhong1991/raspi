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


void navdata_init();
void nav_thread_exit();


#endif
