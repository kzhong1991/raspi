#include "video.h"

AVFormatContext *pFormatCtx;
int video_stop = 0;
int video_stream_index;
char *file_prefix = "data/video_";
int file_index = 0;
pthread_t video_pid = 0, video_init_pid = 0;



int get_video()
{
    char filename[64];
    AVPacket packet;
    AVStream *stream = NULL;
    int cnt = 0;
    
    av_init_packet(&packet);
    memset(filename, '\0', sizeof(filename));
    sprintf(filename, "%s%d.avi", file_prefix, file_index++);
    AVOutputFormat *ofmat = av_guess_format(NULL, filename, NULL);
    AVFormatContext *ofctx = avformat_alloc_context();
    ofctx->oformat = ofmat;
    avio_open2(&ofctx->pb, filename, AVIO_FLAG_WRITE, NULL, NULL);

    av_read_play(pFormatCtx);
    while (av_read_frame(pFormatCtx, &packet) >= 0 && cnt < MAX_FRAMES_PER_VIDEO) {
        if (packet.stream_index == video_stream_index) {
            if (stream == NULL) {
                stream = avformat_new_stream(ofctx, (AVCodec *) pFormatCtx->streams[video_stream_index]->codec->codec);
                avcodec_copy_context(stream->codec, pFormatCtx->streams[video_stream_index]->codec);
                stream->sample_aspect_ratio = pFormatCtx->streams[video_stream_index]->codec->sample_aspect_ratio;
                avformat_write_header(ofctx, NULL);
            }
            packet.stream_index = stream->id;
            av_write_frame(ofctx, &packet);
            cnt++;
        }

        av_free_packet(&packet);
        av_init_packet(&packet);
    }
   
    av_read_pause(pFormatCtx); 
    av_write_trailer(ofctx);
    avio_close(ofctx->pb);
    avformat_free_context(ofctx);

    printf(">>>>>>video file %s saved, %d frames.\n", filename, cnt);
}

static void *video_worker(void *arg)
{
    while (!video_stop){
       get_video();     
    }
}

void *video_init_worker(void *arg)
{
    char video_addr[256]; 
    int i;

    printf("\033[1;33m""Initing video...\n""\033[0m");
    
    // Register all formats and codecs
    av_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_QUIET);

    pFormatCtx = avformat_alloc_context();
    memset(video_addr, '\0', sizeof(video_addr));   
    sprintf(video_addr, "tcp://%s:%d", get_ardrone_ip(), VIDEO_PORT);
    if (avformat_open_input(&pFormatCtx, video_addr, NULL, NULL) < 0) {
        printf(">>>>>>avformat_open_input() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

    // Retrive and dump stream information
    avformat_find_stream_info(pFormatCtx, NULL);
    av_dump_format(pFormatCtx, 0, video_addr, 0);

     // search the video stream
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
    }
    
    /* create the video reciving thread */
    if (pthread_create(&video_pid, NULL, video_worker, NULL) != 0) {
        printf(">>>>>> create video thread failed.\n");
        return 0;
    }

    printf("\033[1;33m""Init video OK.\n""\033[0m");    

}

int video_init()
{
    int err;
    err = pthread_create(&video_init_pid, NULL, video_init_worker, NULL);
    if (err != 0) {
        printf(">>>>>>create video init thread failed.\n");
        return -1;
    }
    return 0;
}

void video_thread_exit()
{
    video_stop = 1;   
}

