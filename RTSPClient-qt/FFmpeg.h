/*
 * FFmpeg.h
 *
 *  Created on: 2014年2月25日
 *      Author: ny
 */

#ifndef FFMPEG_H_
#define FFMPEG_H_
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}
#include <QReadWriteLock>
#include <QMutex>
#include <QString>



class FFmpeg
{
public:
    FFmpeg();
    int initial(QString & url);
    int h264Decodec();
    virtual ~FFmpeg();
    friend class Video;
private:
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVFrame *pFrame;
    int videoStream;
    int width;
    int height;
    QMutex mutex;
    QString rtspURL;

    uint8_t *pictureData[AV_NUM_DATA_POINTERS];

};

#endif /* FFMPEG_H_ */
