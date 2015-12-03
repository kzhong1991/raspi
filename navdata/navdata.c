#include "navdata.h"


int nav_socket;
struct sockaddr_in dst_addr, src_addr;
pthread_t nav_pid = NULL;


static int nav_socket_init()
{
    int res;
    char *ardrone_ip;

    ardrone_ip = get_ardrone_ip();

    bzero(&src_addr, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    src_addr.sin_port = htons(NAV_DATA_PORT);

    bzero(&dst_addr, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_addr.s_addr = inet_addr(ardrone_ip);
    dst_addr.sin_port = htons(NAV_DATA_PORT);
    
    nav_socket = socket(AF_INET, SOCK_DGRAM, 0);
    res = bind(nav_socket, (struct sockaddr*)&src_addr, sizeof(src_addr));
    if (!res)
        printf("Binding socket to NAV_DATA_PORT FAILED!\n");
    else
        printf("Binding socket to NAV_DATA_PORT OK!\n");

    return res;

}

static inline void send_datagram(char *data, int len)
{
    sendto(nav_socket, data, len, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr));
}

static inline int recv_datagram(char *msg)
{
    int len = sizeof(src_addr);
    return recvfrom(nav_socket, msg, sizeof(msg), 0, (struct sockaddr*)&src_addr, &len);
}

static int is_nav_pkt()
{
    return 0;
}

void navdata_init()
{
    char data[5] = {0x01, 0x01, 0x01, 0x01, 0x00};
    char msg[4096];
    memset(msg, '\0', sizeof(4096));

    nav_socket_init();

    send_datagram(data, strlen(data));
    
    while (1) {
        //recv_datagram(msg, )
    }
   
}

void nav_thread_exit()
{
    //stop = 1;
}
