/************************************************
 *@file     board.h
 *@author    JY
 *@date     15/12/03
 *@brief    to get data from coordinator 
*************************************************/
#ifndef BOARD_H
#define BOARD_H
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include "datastruct.h"
#define MAX_DATA_SIZE  (30)
void board_init();


#endif
