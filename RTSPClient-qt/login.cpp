#include "login.h"
#include "ui_login.h"
#include <qstring.h>
#include <stdio.h>
#include <QString>

extern bool h264DecodecStop;


Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btn_play_clicked()
{
    h264DecodecStop = false;
    rtspURL=QString(ui->rtspURL->text());

}
QString Login::getRtspURL()
{

    return rtspURL;
}
