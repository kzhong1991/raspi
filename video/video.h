#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <pthread.h>
// OpenCV
#include "opencv2/opencv.hpp"
// FFmpeg
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
int init_video(void);
int get_video(void);


