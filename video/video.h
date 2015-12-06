#ifndef VIDEO_H
#define VIDEO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
// OpenCV
//#include<opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>  
//#include <opencv2/highgui/highgui.hpp>  
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include"../common/ardrone.h"

#define MAX_FRAMES_PER_VIDEO (2000) 

int video_init(void);
void video_thread_exit();

#endif

