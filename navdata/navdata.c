#include "navdata.h"


int nav_socket;
struct sockaddr_in nav_dst_addr, nav_src_addr;
pthread_t nav_pid = NULL;
int nav_stop = 0;

static int nav_socket_init()
{
    int res;
    char *ardrone_ip;
    struct timeval tv_out;
    tv_out.tv_sec = 3;
    tv_out.tv_usec = 0;

    ardrone_ip = get_ardrone_ip();

    bzero(&nav_src_addr, sizeof(nav_src_addr));
    nav_src_addr.sin_family = AF_INET;
    nav_src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    nav_src_addr.sin_port = htons(NAV_DATA_PORT);

    bzero(&nav_dst_addr, sizeof(nav_dst_addr));
    nav_dst_addr.sin_family = AF_INET;
    nav_dst_addr.sin_addr.s_addr = inet_addr(ardrone_ip);
    nav_dst_addr.sin_port = htons(NAV_DATA_PORT);
    
    nav_socket = socket(AF_INET, SOCK_DGRAM, 0);
    res = bind(nav_socket, (struct sockaddr*)&nav_src_addr, sizeof(nav_src_addr));
    if (res != 0)
        printf(">>>>>>binding nav socket to NAV_DATA_PORT FAILED!\n");
    else
        printf(">>>>>>binding nav socket to NAV_DATA_PORT OK!\n");

    setsockopt(nav_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_out, sizeof(tv_out));
    

    return res;

}

void *nav_worker(void *arg)
{
    while (!nav_stop) {
        printf("In worker thread\n");
        usleep(CMD_INTERVAL); 
    }
}

static void  nav_thread_init()
{
    int err;
    err = pthread_create(&nav_pid, NULL, nav_worker, NULL);
    if (err != 0)
        printf("create nav thread error: %s\n", strerror(err));
}

static inline void send_datagram(char *data, int len)
{
    int res;
    res = sendto(nav_socket, data, len, 0, (struct sockaddr *)&nav_dst_addr, sizeof(nav_dst_addr));
    printf("send %d bytes\n", res);
}

static inline int recv_datagram(char *msg)
{
    int len = sizeof(nav_dst_addr);
    int rc;
    rc = recvfrom(nav_socket, msg, sizeof(msg), 0, (struct sockaddr*)&nav_dst_addr, &len);
    if (rc == -1) printf("error: %s\n", strerror(errno));
    return rc;
}

static int is_nav_pkt(char *msg)
{
   if (*((uint32_t*)msg) == NAVDATA_HEADER)
       return 1;
   else
       return 0;
}
static uint32_t get_state(char *msg)
{
    msg += NAVDATA_STATE_OFFSET;
    return *((uint32_t*)msg);
}

void navdata_init()
{
    char data[5] = {0x01, 0x01, 0x01, 0x01, 0x00};
    char msg[4096], cmd[1024], tmp[64];
    int res = 0;
    int try = 0;
    uint32_t state;

    memset(msg, '\0', sizeof(msg));
    memset(cmd, '\0', sizeof(cmd));
    memset(tmp, '\0', sizeof(tmp));

    printf("Initing navdata...\n");

    nav_socket_init();

    while (1) {
try_again:
        send_datagram(data, strlen(data));
        res = recv_datagram(msg);
        if (res == -1) {
            printf(">>>>>>receive ardrone status TIMEOUT!\n");
            try++;
            if (try >= 3) {
                printf("Init navdata failed.\n");
                cmd_thread_exit();
                return;
            } else
                goto try_again;
        } else {
            if (!is_nav_pkt(msg))
                continue;
            
            state = get_state(msg);

            if (state & ARDRONE_COMMAND_MASK) {
                sprintf(cmd, "AT*CTRL=%d,5,0\r", get_seq_num());
                send_AT_cmd(cmd);
            }

            memset(msg, '\0', sizeof(msg));
            sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", get_seq_num(), 
                    ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
            sprintf(tmp, "AT*CONFIG=%d,\"general:navdata_demo\",\"FALSE\"\r", get_seq_num());
            strcat(cmd, tmp);
            send_AT_cmd(cmd);
            break;
        }
    }

    try = 0;
    while (1) {
        res = recv_datagram(msg);
        if (-1 == res) {
            printf(">>>>>>receive ardrone status TIMEOUT!\n");
            try++;
            if (try >= 3) {
                printf("Init navdata failed.\n");
                cmd_thread_exit();
                return;
            } else {
                memset(msg, '\0', sizeof(msg));
                memset(tmp, '\0', sizeof(tmp));
                sprintf(cmd, "AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", get_seq_num(), 
                        ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLICATION_ID);
                sprintf(tmp, "AT*CONFIG=%d,\"general:navdata_demo\",\"FALSE\"\r", get_seq_num());
                strcat(cmd, tmp);
                send_AT_cmd(cmd);
            }
        } else {
            if (!is_nav_pkt(msg))
                continue;
            state = get_state(msg);

            if (state & ARDRONE_COMMAND_MASK && !(state & ARDRONE_NAVDATA_DEMO_MASK)) {
                sprintf(cmd, "AT*CTRL=%d,5,0\r", get_seq_num());
                send_AT_cmd(cmd);
                break;
            }
        }
    }

    printf("Init navdata OK.\n");
    if (!nav_pid)
        nav_thread_init();
}

void nav_thread_exit()
{
    nav_stop = 1;
}
