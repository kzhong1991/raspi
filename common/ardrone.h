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
#include <errno.h>

/* Receive nvdata on UPD port 5554 */
#define NAV_DATA_PORT   (5554)

/* Reveive video stream on TCP port 5555 */
#define VIDEO_PORT      (5555)

/* Send AT commands on UPD port 5556 */
#define AT_CMD_PORT     (5556)

/* retrieve configurtion data, or send iimportant information */
#define CONTROL_PORT    (5559)

#define CMD_INTERVAL    (200)


#define ARDRONE_SESSION_ID      "d2e081a3"
#define ARDRONE_PROFILE_ID      "be27e2e4"
#define ARDRONE_APPLICATION_ID  "d87f7e0c"

/*
#define ARDRONE_SESSION_ID      "d2e081a3"
#define ARDRONE_PROFILE_ID      "be27e2a4"
#define ARDRONE_APPLICATION_ID  "d87f7eac"
*/

#define ARDRONE_IP              "192.168.1.1"

/******** Available codecs ********/
typedef enum _codec_type_t {
	NULL_CODEC    = 0,
	UVLC_CODEC    = 0x20,       // codec_type value is used for START_CODE
	MJPEG_CODEC,                // not used
	P263_CODEC,                 // not used
	P264_CODEC    = 0x40,
	MP4_360P_CODEC = 0x80,
	H264_360P_CODEC = 0x81,
	MP4_360P_H264_720P_CODEC = 0x82,
	H264_720P_CODEC = 0x83,
	MP4_360P_SLRS_CODEC = 0x84,
	H264_360P_SLRS_CODEC = 0x85,
	H264_720P_SLRS_CODEC = 0x86,
	H264_AUTO_RESIZE_CODEC = 0x87,    // resolution is automatically adjusted according to bitrate
	MP4_360P_H264_360P_CODEC = 0x88,

} codec_type_t;


void ardrone_init();
void send_AT_cmd(char *cmd);
int get_seq_num();
void wait_thread_loop();
void cmd_thread_exit();
char *get_ardrone_ip();

#endif

