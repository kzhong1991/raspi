/**********************************************************
 * @file    ardrone.h
 * @author  kzhong1991<kzhong1991@gmail.com>
 * @date    2015-12-03
 * @brief   ardrone control API
 *********************************************************/
#ifndef ARDRONE_H
#define ARDRONE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

/* Receive nvdata on UPD port 5554 */
#define NAV_DATA_PORT   (5554)

/* Reveive video stream on TCP port 5555 */
#define VIDEO_PORT      (5555)

/* Send AT commands on UPD port 5556 */
#define AT_CMD_PORT     (5556)

/* retrieve configurtion data, or send iimportant information */
#define CONTROL_PORT    (5559)

#define CMD_INTERVAL    (200)

#define ARDRONE_SESSION_ID      "75f2dcd6"
#define ARDRONE_PROFILE_ID      "00000000"
#define ARDRONE_APPLICATION_ID  "96eb3af0"

#define ARDRONE_IP              "192.168.1.1"

void ardrone_init();
void send_AT_cmd(const char *cmd);
void ardrone_config(const char *name, const char *value);
void send_PCMD_MAG(int enable, float roll, float pitch, float gaz, float yaw, float psi, float psi_accuracy);
int get_seq_num();
void wait_thread_loop();
void cmd_thread_exit();
char *get_ardrone_ip();

#endif

