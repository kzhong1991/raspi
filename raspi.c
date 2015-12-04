#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common/ardrone.h"
#include "navdata/navdata.h"
#include "board/board.h"

int main(int argc, char *argv[])
{
    printf("Ardrone data gathering program, %s, %s\n", __DATE__, __TIME__);
    xxxx_init();
    ardrone_init();
    navdata_init();
    //video_init();
    wait_thread_loop();
    return 0;
}
