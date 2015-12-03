#include"video.h"
AVFormatContext *pFormatCtx;
AVCodecContext  *pCodecCtx;
AVFrame         *pFrame, *pFrameBGR;
uint8_t         *bufferBGR;
SwsContext      *pConvertCtx;
IplImage *img;
VideoWriter writer("/home/saber/模板/raspi/video/VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(360, 480));
int init_video(void){
        char filename[256];
        sprintf(filename, "tcp://%s:%d", ip, ARDRONE_VIDEO_PORT);
        if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) < 0) {
            CVDRONE_ERROR("avformat_open_input() was failed. (%s, %d)\n", __FILE__, __LINE__);
            return 0;
        }

        // Retrive and dump stream information
        avformat_find_stream_info(pFormatCtx, NULL);
        av_dump_format(pFormatCtx, 0, filename, 0);

        // Find the decoder for the video stream
        pCodecCtx = pFormatCtx->streams[0]->codec;
        AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (pCodec == NULL) {
            CVDRONE_ERROR("avcodec_find_decoder() was failed. (%s, %d)\n", __FILE__, __LINE__);
            return 0;
        }

        // Open codec
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
            CVDRONE_ERROR("avcodec_open2() was failed. (%s, %d)\n", __FILE__, __LINE__);
            return 0;
        }

        // Allocate video frames and a buffer
        pFrame = avcodec_alloc_frame();
        pFrameBGR = avcodec_alloc_frame();
        bufferBGR = (uint8_t*)av_mallocz(avpicture_get_size(PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height) * sizeof(uint8_t));

        // Assign appropriate parts of buffer to image planes in pFrameBGR
        avpicture_fill((AVPicture*)pFrameBGR, bufferBGR, PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);

        // Convert it to BGR
        pConvertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_BGR24, SWS_SPLINE, NULL, NULL, NULL);
    }
    // AR.Drone 1.0
    else {
        // Open the IP address and port
        if (!sockVideo.open(ip, ARDRONE_VIDEO_PORT)) {
            CVDRONE_ERROR("UDPSocket::open(port=%d) was failed. (%s, %d)\n", ARDRONE_VIDEO_PORT, __FILE__, __LINE__);
            return 0;
        }

        // Set codec
        pCodecCtx = avcodec_alloc_context();
        pCodecCtx->width = 320;
        pCodecCtx->height = 240;

        // Allocate a buffer
        bufferBGR = (uint8_t*)av_mallocz(avpicture_get_size(PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height));
    }

    // Allocate an IplImage
    img = cvCreateImage(cvSize(pCodecCtx->width, (pCodecCtx->height == 368) ? 360 : pCodecCtx->height), IPL_DEPTH_8U, 3);
    if (!img) {
        CVDRONE_ERROR("cvCreateImage() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

    // Clear the image
    cvZero(img);
      while (1) {
        if (!getVideo()) break;
    }
    cvReleaseVideoWriter(&writer);
}
int get_video(void)
{
    // AR.Drone 2.0
        AVPacket packet;
        int frameFinished = 0;
       
        // Read all frames
        while (av_read_frame(pFormatCtx, &packet) >= 0) {
            // Decode the frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Decoded all frames
            if (frameFinished) {
                // Convert to BGR
                sws_scale(pConvertCtx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameBGR->data, pFrameBGR->linesize);
                memcpy(img->imageData, pFrameBGR->data[0], pCodecCtx->width * ((pCodecCtx->height == 368) ? 360 : pCodecCtx->height) * sizeof(uint8_t) * 3);
                // Free the packet and break immidiately
                cvWriteFrame(writer,img); // add the frame to the file
                av_free_packet(&packet);
                break;
            }

            // Free the packet
            av_free_packet(&packet);
        }

    return 1;
}

