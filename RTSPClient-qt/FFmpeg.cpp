#include "FFmpeg.h"
#include <QDebug>

extern bool h264DecodecStop;

FFmpeg::FFmpeg()
{
    pCodecCtx = NULL;
    videoStream=-1;
    for(int i=0; i <AV_NUM_DATA_POINTERS; i++)
    {
        pictureData[i] = {0};
    }

}

FFmpeg::~FFmpeg()
{
}

int FFmpeg::initial(QString & url)
{
    qDebug() << "FFmpeg initial start.";

    int ret;
    rtspURL=url;
    const AVCodec *pCodec;
    //初始化
    //av_register_all();        //新版的不需要注册
    avformat_network_init();    //支持网络流

    //打开码流前，设置参数
    AVDictionary *optionsDict = NULL;
    av_dict_set(&optionsDict, "rtsp_transport", "tcp", 0);
    av_dict_set(&optionsDict, "stimeout", "30000000", 0);
    pFormatCtx = avformat_alloc_context();  //初始化AVFormatContext, 分配一块内存，保存视频属性信息
    ret = avformat_open_input(&pFormatCtx, rtspURL.toStdString().c_str(), NULL, &optionsDict);
    char buf[1024] = {0};
    if (ret < 0)
    {
        qDebug() << "Can not open this file";
        av_strerror(ret, buf, 1024);
        return -1;
    }

    //查找流信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        qDebug() << "Unable to get stream info";
        return -1;
    }

    //获取视频流ID
    int i = 0;
    videoStream = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1)
    {
        qDebug() << "Unable to find video stream";
        return -1;
    }

    //AVCodecParameters转AVCodecContext
    pCodecCtx = avcodec_alloc_context3(NULL);
    avcodec_parameters_to_context(pCodecCtx,pFormatCtx->streams[videoStream]->codecpar);

    //查找解码器
    pCodec    = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
    {
        qDebug() << "Unsupported codec";
        return -1;
    }

    //打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        qDebug() << "Unable to open codec";
        return -1;
    }

    //打印关于输入或输出格式的详细信息
    av_dump_format(pFormatCtx, 0, rtspURL.toStdString().c_str(), 0);

    //存储解码后AVFrame
    pFrame = av_frame_alloc();

    height = pCodecCtx->height;
    width = pCodecCtx->width;

    qDebug() << "FFmpeg initial successfully";


    return 0;
}


int FFmpeg::h264Decodec()
{
    qDebug() << "h264Decodec start...";

    //为packet分配内存
    AVPacket * packet = NULL;
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    //循环 获取视频的数据
    while (av_read_frame(pFormatCtx, packet) >= 0)
    {
        //停止读取视频流
        if(h264DecodecStop)
        {
            //进行内存释放
            av_packet_unref(packet);
            break;
        }

        //当视频流的ID跟读取到的视频流ID一致的时候
        if(packet->stream_index==videoStream)
        {
            //解码
            int re = avcodec_send_packet(pCodecCtx, packet);
            av_packet_unref(packet);
            if(re!=0)
            {
               continue;
            }

            //从解码器中获取解码的输出数据，存入pFrame
            re = avcodec_receive_frame(pCodecCtx, pFrame);
            if(re!=0)
            {
                qDebug() << "avcodec_receive_frame failed !";
               continue;
            }

            //初始化img_convert_ctx
            struct SwsContext * img_convert_ctx;
            img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
            if (img_convert_ctx == NULL)
            {
                break;
            }

            mutex.lock();

            char *rgb = new char[width*height*3];
            memset(rgb, 0 , width*height*3);
            pictureData[0] = (uint8_t *)rgb;
            int lines[AV_NUM_DATA_POINTERS] = {0};
            lines[0] = width * 3;

            //开始转换图像数据
            int rs = 0;
            rs = sws_scale(img_convert_ctx,
                                (const uint8_t**) pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                pictureData, lines);

            if (img_convert_ctx)
            {
                sws_freeContext(img_convert_ctx);
                img_convert_ctx = NULL;
            }

            mutex.unlock();
            if (rs == -1)
            {
                qDebug() << "__________Can open to change to des imag_____________e\n";
                return -1;
            }

        }
    }

    if (packet!=NULL)
    {
        av_packet_unref (packet);
        packet = NULL;
    }
    if(pFrame!=NULL)
    {
        av_frame_free(&pFrame);
        pFrame = NULL;
    }

    qDebug() << "h264Decodec end...";
    return 0;
}
