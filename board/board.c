#include "board.h"

int board_socket;
struct sockaddr_in board_addr;
pthread_t board_pid = NULL ;
int board_stop = 0;
board_data_t date_t;

static int board_socket_init()
{
    printf("@@@@@@@@@@init boardsocket@@@@@@@@@@@\n");

	if ((board_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
     {
             fprintf(stderr, "socket error!\n");
             return 0;
        }

     board_addr.sin_family = AF_INET;
     board_addr.sin_port = htons(BOARD_PORT);
     board_addr.sin_addr.s_addr = inet_addr("172.21.161.199");
     bzero(&(board_addr.sin_zero), 8);


    if (connect(board_socket, (struct sockaddr *)&board_addr, sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr,"connect error!\n");
        return 0;
    } 
    fprintf(stdout, "connect ok!\n");
	return 1;
}

static board_data_t makedata(char *buf)
{
	int i = 0;
	int index =0;
	board_data_t data_t;
	for(i=0;i<5;i++)
	{
		data_t.id = i;
		data_t.temp  = buf[index];
		data_t.humi  = buf[index+1];
		data_t.light = buf[index+2];
		data_t.gas   = buf[index+3];
		index += 4;
		if(i>=1)
		{
			printf("endpiont %2d : ",data_t.id);
			printf("%02x ",((char *)data_t.temp));
			printf("%02x ",((char *)data_t.humi));
			printf("%02x ",((char *)data_t.light));
			printf("%02x ",((char *)data_t.gas));
			printf("\n");
		}
	}
	return data_t; 
}




void *board_worker(void *arg)
{
	unsigned char buf[MAX_DATA_SIZE];

	char sendMessage[6]={0x3A,0x00,0xFF,0X01,0XC4,0X23};

	while(1)
	{
		int i,numbytes=0;
		printf("&&&&&&&&&&&test&&&&&&&&&&&&&\n");
		while(1){
			send(board_socket, &sendMessage, sizeof(sendMessage), 0);
			if ((numbytes = read(board_socket , buf, MAX_DATA_SIZE)) == -1)
        	{
           		fprintf(stderr, "read error!\n");
           		return 0;
        	}
	   		if(numbytes == 22 && buf[0]==0x3A)
    		{
        		fprintf(stdout, "numbytes=%d\n",numbytes);
//            	for(i=0; i<numbytes; i++)
//                	printf("%02x ",((char *)buf)[i]);
				makedata(buf);
				break;				
         	}
    	}
	}
} 


void board_init()
{
	int err;
	printf("***************board***********\n");
	while(!board_socket_init())
		printf("socket init error!\n");
	err = pthread_create(&board_pid, NULL,
			board_worker ,NULL);
	if(err !=0 )
		printf("create board thread error: %s\n",strerror(err));
	return;
}

