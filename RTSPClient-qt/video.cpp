#include "video.h"
#include "ui_video.h"
#include <QPainter>
#include <QDesktopWidget>
#include <QTime>

extern bool h264DecodecStop;

Video::Video(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Video)
{
    ui->setupUi(this);
    ffmpeg=NULL;

    this->move ((QApplication::desktop()->width() - this->width())/2,(QApplication::desktop()->height() - this->height())/2);
}

Video::~Video()
{
    delete ui;
}
void Video::setFFmpeg(FFmpeg *ff)
{
    ffmpeg=ff;
}

void Video::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(ffmpeg->mutex.tryLock(1000))
    {
        QImage image=QImage(ffmpeg->pictureData[0],ffmpeg->width,ffmpeg->height,QImage::Format_RGB888);
        QPixmap  pix =  QPixmap::fromImage(image);
        painter.drawPixmap(0, 0, 640, 480, pix);
        update();
        ffmpeg->mutex.unlock();
    }
}

void mSleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    }

}

void Video::closeEvent(QCloseEvent *event)
{
    h264DecodecStop = true;
    mSleep(500);
}
