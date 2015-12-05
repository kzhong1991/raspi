/**********************************************************************
 * @file    datastruct.h
 * @author  JY
 * @date    15/12/03
 * @brief   the struct of board
 **********************************************************************/
#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include <stdint.h>

#define BOARD_PORT 33333
#define BOARD_IP "172.21.161.199"

typedef struct board_data_t {
	int id;
	int temp;
	int humi;
	int light;
	int gas;
} board_data_t;

#endif 
