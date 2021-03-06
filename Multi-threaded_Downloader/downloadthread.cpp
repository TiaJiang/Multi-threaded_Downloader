#include "downloadthread.h"
#include <QDebug>

DownloadThread::DownloadThread(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    state = Waiting;
    index = 0;
}

//利用RANGE头部，支持了断点续传
void DownloadThread::startDownload(int _index, QString _url, QFile *_file, qint64 _startPoint, qint64 _endPoint, qint64 _readySize)
{
    if(state == Downloading)
    {
        errorInfo = "is downloading a file";
        emit error(index,errorInfo);
        return;
    }

    index = _index;
    url = _url;
    file = _file;
    startPoint = _startPoint;
    endPoint = _endPoint;
    readySize = _readySize;

    //根据HTTP协议，**写入RANGE头部，说明请求文件的范围**
    QNetworkRequest request;
    request.setUrl(_url);
    QString range = QString( "bytes=%0-%1" ).arg( startPoint + readySize ).arg( endPoint );
    request.setRawHeader("Range", range.toLatin1());

    reply = manager->get(request);
    connect(reply,SIGNAL(finished()),SLOT(finishedSlot()));
    connect(reply,SIGNAL(readyRead()),SLOT(readyReadSlot()));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(errorSlot(QNetworkReply::NetworkError)));

    state = Downloading;
    //向Thread发出进度条改变信号
    emit progressChange(index,startPoint,endPoint,readySize);
}

DownloadThread::State DownloadThread::getState()
{
    return state;
}

QString DownloadThread::errorString()
{
    return errorInfo;
}

//该线程停止,并删除reply
void DownloadThread::stop()
{
    if(state == Finished) return;
    if(state != Downloading)
    {
        errorInfo = "is not downloading";
        emit error(index,errorInfo);
        return;
    }
    state = Stoped;

    reply->abort();
    file->flush();
    reply->deleteLater();
}

//调用startDownload()
void DownloadThread::restart()
{
    if(state == Finished) return;

    if(state != Stoped)
    {
        errorInfo = "is not stoped";
        emit error(index,errorInfo);
        return;
    }

    startDownload(index,url,file,startPoint,endPoint,readySize);
}


void DownloadThread::finishedSlot()
{
    if(state != Downloading)return;
    file->flush();
    reply->deleteLater();
    state = Finished;
    emit finished(index);
}

void DownloadThread::readyReadSlot()
{
    //将读到的信息写入文件
    QByteArray buffer = reply->readAll();
    file->seek( startPoint + readySize );
    file->write(buffer);
    readySize += buffer.size();

    //发出进度条改变信号
    emit progressChange(index,startPoint,endPoint,readySize);
}

void DownloadThread::errorSlot(QNetworkReply::NetworkError code)
{
    if(state != Downloading)return;
    errorInfo = "QNetworkReply::NetworkError : " + QString::number((int)code) + " \n" + reply->errorString();
    stop();
    //发出错误信号
    emit error(index,errorInfo);
}
