#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common/ardrone.h"
#include "navdata/navdata.h"

int main(int argc, char *argv[])
{
    printf("Ardrone data gathering program, %s, %s\n", __DATE__, __TIME__);
    ardrone_init();
    navdata_init();
    /*
     * add nvdata and video code here
     */
    wait_thread_loop();
    return 0;
}
