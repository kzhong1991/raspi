#include "ardrone.h"

int seq_num = 0;
int cmd_socket = 0;
struct sockaddr_in cmd_dst_addr, cmd_src_addr;
pthread_mutex_t mutex;
pthread_t cmd_pid = NULL;
int cmd_stop = 0;

static int cmd_socket_init()
{
    int res;
    char *ardrone_ip;

    if (cmd_socket != 0)
        return 0;

    ardrone_ip = get_ardrone_ip();

    bzero(&cmd_src_addr, sizeof(cmd_src_addr));
    cmd_src_addr.sin_family = AF_INET;
    cmd_src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    cmd_src_addr.sin_port = htons(AT_CMD_PORT);

    bzero(&cmd_dst_addr, sizeof(cmd_dst_addr));
    cmd_dst_addr.sin_family = AF_INET;
    cmd_dst_addr.sin_addr.s_addr = inet_addr(ardrone_ip);
    cmd_dst_addr.sin_port = htons(AT_CMD_PORT);
    
    cmd_socket = socket(AF_INET, SOCK_DGRAM, 0);
    res = bind(cmd_socket, (struct sockaddr*)&cmd_src_addr, sizeof(cmd_src_addr));
    if (res != 0)
        printf(">>>>>>binding cmd socket to AT_CMD_PORT FAILED!\n");
    else
        printf(">>>>>>binding cmd socket to AT_CMD_PORT OK!\n");
       
    return res;

}

static void *cmd_worker(void *arg)
{
    char cmd[1024], tmp[64];
    
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    while (!cmd_stop) {
        sprintf(cmd, "AT*COMWDG=%d\r", get_seq_num());
        send_AT_cmd(cmd);
        usleep(CMD_INTERVAL * 5);       
    }
}

static void cmd_socket_close()
{
    close(cmd_socket);
}

void cmd_thread_init()
{
    int err;
    err = pthread_create(&cmd_pid, NULL, cmd_worker, NULL);
    if (err != 0)
        printf("create cmd thread error: %s\n", strerror(err));
}

char *get_ardrone_ip()
{
    return ARDRONE_IP;
}

void ardrone_init()
{
    char cmd[1024], tmp[64];

    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    printf("Connecting ardrone...\n");
    pthread_mutex_init(&mutex, NULL);
    cmd_socket_init();   

    sprintf(cmd, "AT*PMODE=%d,2\r", get_seq_num());
    send_AT_cmd(cmd);

    sprintf(cmd, "AT*MISC=%d,2,20,2000,3000\r", get_seq_num());
    send_AT_cmd(cmd);

    sprintf(cmd, "AT*FTRIM=%d\r", get_seq_num());
    send_AT_cmd(cmd);

    sprintf(cmd, "AT*REF=%d,290717696\r",get_seq_num());
    send_AT_cmd(cmd);

    sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r",get_seq_num(), 
            ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    send_AT_cmd(cmd);
    sprintf(cmd, "AT*CONFIG=%d,\"custom:session_id\",\"%s\"\r", get_seq_num(), ARDRONE_SESSION_ID);
    send_AT_cmd(cmd);

    sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r",get_seq_num(), 
            ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    send_AT_cmd(cmd);
    sprintf(cmd, "AT*CONFIG=%d,\"custom:profile_id\",\"%s\"\r", get_seq_num(), ARDRONE_PROFILE_ID);
    send_AT_cmd(cmd);

    sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r",get_seq_num(), 
            ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    send_AT_cmd(cmd);
    sprintf(cmd, "AT*CONFIG=%d,\"custom:application_id\",\"%s\"\r", get_seq_num(), ARDRONE_APPLICATION_ID);
    send_AT_cmd(cmd);

    /* Set video codec */
	sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", get_seq_num(), ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    send_AT_cmd(cmd);
	sprintf(cmd, "AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", get_seq_num(), H264_360P_CODEC);
    send_AT_cmd(cmd);

	/* Set to default video channel */
	sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", get_seq_num(), ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    send_AT_cmd(cmd);
	sprintf(cmd, "AT*CONFIG=%d,\"video:video_channel\",\"0\"\r", get_seq_num());
    send_AT_cmd(cmd);
    


    /*
    sprintf(cmd, "AT*PMODE=%d,2\r", get_seq_num());
    sprintf(tmp, "AT*MISC=%d,2,20,2000,3000\r", get_seq_num());
    strcat(cmd, tmp);

    sprintf(tmp, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r",get_seq_num(), 
            ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID); 
    strcat(cmd, tmp);

    sprintf(tmp, "AT*CONFIG=%d,\"custom:session_id\",\"-all\"\r", get_seq_num());
    strcat(cmd, tmp);

    sprintf(tmp, "AT*PCMD_MAG=%d,0,0,0,0,0,0,0\r", get_seq_num());
    strcat(cmd, tmp);

    sprintf(tmp, "AT*REF=%d,290717696\r",get_seq_num());
    strcat(cmd, tmp);
       
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));


    sprintf(cmd, "AT*PCMD_MAG=%d,0,0,0,0,0,0,0\r", get_seq_num());
    sprintf(tmp, "AT*REF=%d,290717696\r", get_seq_num());
    strcat(cmd, tmp);
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    sprintf(cmd, "AT*PCMD_MAG=%d,0,0,0,0,0,0,0\r", get_seq_num());
    sprintf(tmp, "AT*REF=%d,290717696\r", get_seq_num());
    strcat(cmd, tmp);
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    sprintf(cmd, "AT*PCMD_MAG=%d,0,0,0,0,0,0,0\r", get_seq_num());
    sprintf(tmp, "AT*REF=%d,290717696\r", get_seq_num());
    strcat(cmd, tmp);
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));


    sprintf(cmd, "AT*CTRL=%d,5\r", get_seq_num());
    sprintf(tmp, "AT*CTRL=%d,5\r", get_seq_num());
    strcat(cmd, tmp);
    sprintf(tmp, "AT*PCMD_MAG=%d,0,0,0,0,0,0,0\r", get_seq_num());
    strcat(cmd, tmp);
    sprintf(tmp, "AT*REF=%d,290717696\r", get_seq_num());
    strcat(cmd, tmp);
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r",get_seq_num(), 
            ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    sprintf(tmp, "AT*CONFIG=%d,\"custom:session_id\",\"%s\"\r", get_seq_num(), ARDRONE_SESSION_ID);
    strcat(cmd, tmp);
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r",get_seq_num(), 
            ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    sprintf(tmp, "AT*CONFIG=%d,\"custom:profile_id\",\"%s\"\r", get_seq_num(), ARDRONE_PROFILE_ID);
    strcat(cmd, tmp);
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));


    sprintf(cmd, "AT*CONFIG=%d,\"%s\",\"%s\",\"%s\"\r",get_seq_num(), 
            ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
    sprintf(tmp, "AT*CONFIG=%d,\"custom:application_id\",\"%s\"\r", get_seq_num(), ARDRONE_APPLICATION_ID);
    strcat(cmd, tmp);
    send_AT_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));
    */

    if (!cmd_pid)
        cmd_thread_init();
}

void wait_thread_loop()
{
    pthread_join(cmd_pid, NULL);
    printf("cmd thread exit.\n");
}


void send_AT_cmd(char *cmd)
{
    int res;

    res = sendto(cmd_socket, cmd, strlen(cmd), 0, (struct sockaddr *)&cmd_dst_addr, sizeof(cmd_dst_addr));

    if (res == -1) {
        printf("Sending AT command [%s] FAILD.\n", cmd);
    }
    memset(cmd, '\0', sizeof(cmd));
    usleep(CMD_INTERVAL);
}

int get_seq_num()
{
    pthread_mutex_lock(&mutex);
    seq_num++;
    pthread_mutex_unlock(&mutex);
    return seq_num;
}

void cmd_thread_exit()
{
   cmd_stop = 1;
}
