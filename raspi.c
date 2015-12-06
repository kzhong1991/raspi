#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common/ardrone.h"
#include "navdata/navdata.h"
#include "video/video.h"
#include "board/board.h"

int main(int argc, char *argv[])
{
    printf("Sensor data collecting program\n Build time: %s, %s, version 1.0 \n", __DATE__, __TIME__);
    ardrone_init();
    navdata_init();
    video_init();
    board_init();
    wait_thread_loop();
    return 0;
}
