
#include <QString>
#include "camerasurfaceview.h"
#include <android/log.h>
#include <QPainter>
#include <QQuickItem>
#include <QMetaObject>
const char kJTag[] = "QGCCameraSurfaceView";
QGCCameraSurfaceView::QGCCameraSurfaceView()
{
    mWhileFetching = false;
    _netWorkManager = new QNetworkAccessManager();
        tcpSocket = new QTcpSocket(this);
}
QGCCameraSurfaceView::~QGCCameraSurfaceView()
{
    delete _netWorkManager;
    delete tcpSocket;
}

void QGCCameraSurfaceView::networkTimeout()
{

}
void QGCCameraSurfaceView::networkReplyDownloadProgress(qint64 byteReceived,qint64 byteTotal)
{
    __android_log_print(ANDROID_LOG_INFO, kJTag, "networkReplyDownloadProgress %ld/%ld.",byteReceived,byteTotal);
}

void QGCCameraSurfaceView::networkReplyReadyRead()
{
    __android_log_print(ANDROID_LOG_INFO, kJTag, "networkReplyReadyRead.");
}

void QGCCameraSurfaceView::networkReplyFinished()
{
    if (!reply)
    {
        __android_log_print(ANDROID_LOG_INFO, kJTag, "get reply failed");
        return;
    }

    QByteArray replyArry = reply->readAll();
    QByteArray arry;
    bool getPayLoad = false;
    int index = 0;
    __android_log_print(ANDROID_LOG_INFO, kJTag, "get live view stream size %d.",replyArry.count());
    while (index < replyArry.count()) {
        // Common Header
        int readLength = 1 + 1 + 2 + 4;
        QByteArray commonHeader = replyArry.mid(index,readLength);
        if (commonHeader.isEmpty() || commonHeader.length() != readLength) {
            __android_log_print(ANDROID_LOG_INFO, kJTag, "common header is error");
            return;
        }

        if (commonHeader[0] != 0xFF) {
            __android_log_print(ANDROID_LOG_INFO, kJTag, "Unexpected data format. (Start byte)");
            return;
        }

        index += readLength;
        switch (commonHeader[1]) {
            case 0x12:
            {
                // This is information header for streaming.
                // skip this packet.
                readLength = 4 + 3 + 1 + 2 + 118 + 4 + 4 + 24;
                index += readLength;
                break;
            }
            case 0x01:
            case 0x11:
            {
                // Payload Header
                int readLength = 4 + 3 + 1 + 4 + 1 + 115;
                QByteArray payloadHeader = replyArry.mid(index, readLength);
                index += readLength;
                if (payloadHeader.isEmpty() || payloadHeader.length() != readLength) {
                    __android_log_print(ANDROID_LOG_INFO, kJTag, "Cannot read stream for payload header.");
                    return;
                }
                if (payloadHeader[0] != 0x24 || payloadHeader[1] != 0x35
                        || payloadHeader[2] !=  0x68
                        || payloadHeader[3] !=  0x79) {
                    __android_log_print(ANDROID_LOG_INFO, kJTag, "Unexpected data format. (Start code)");
                    return;
                }
                int jpegSize = payloadHeader.mid(4,3).toInt();
                int paddingSize = payloadHeader.mid(7, 1).toInt();

                // Payload Data
                QByteArray jpegData = replyArry.mid(index,jpegSize);
                index += jpegSize;
                QByteArray paddingData = replyArry.mid(index,paddingSize);
                index += paddingSize;

                QImage frame((uchar *)jpegData.data(), 500, 400,QImage::Format_RGB32 );


                QRectF rect;
                QImage* imageView = (QImage*)(parent()->findChild<QObject *>("cameraImageView"));
                if (!imageView)
                {
                     __android_log_print(ANDROID_LOG_INFO, kJTag, "Cannot find image view.");
                }
                QPainter painter(imageView);
                //bool bRet = QMetaObject::invokeMethod(imageView, "setText", Q_ARG(QString, "world hello"));

                painter.drawImage(rect, frame);
                getPayLoad = true;
                break;
            }
        default:
            break;

        }//end switch

        if (getPayLoad)
        {
            break;
        }

    }

}

void QGCCameraSurfaceView::networkReplyError()
{

}
void QGCCameraSurfaceView::tcpDataReceive()
{
    QByteArray data = QByteArray::fromHex(tcpSocket->readAll());
    __android_log_print(ANDROID_LOG_INFO, kJTag, "live view stream receive",data.toStdString().c_str());
}
bool QGCCameraSurfaceView::start(QString streamUrl) {

    if (streamUrl == "") {
        //Log.e(TAG, "start() streamUrl is null.");
        mWhileFetching = false;
        return false;
    }

    if (mWhileFetching) {
        //Log.w(TAG, "start() already starting.");
        return false;
    }

    mWhileFetching = true;

    /*get stream*/
    QUrl url;
    url.setUrl(streamUrl);
    QNetworkRequest req(url);
    reply = _netWorkManager->get(req);
    //reply->setParent(0);
    bool connRest = false;
   // connect(reply, &QNetworkReply::finished, this, &QGCCameraSurfaceView::networkReplyFinished);
    connRest = connect(reply, &QNetworkReply::readyRead, this, &QGCCameraSurfaceView::networkReplyReadyRead);
    if (!connRest)
    {
         __android_log_print(ANDROID_LOG_INFO, kJTag, "connect slot failed");
    }
    connect(reply, &QNetworkReply::downloadProgress, this, &QGCCameraSurfaceView::networkReplyDownloadProgress);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QGCCameraSurfaceView::networkReplyError);

    /*tcpSocket->connectToHost("192.168.122.1",8080);
    tcpSocket->write("GET /liveview/liveviewstream\r\n\r\n");
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(tcpDataReceive()));*/
    //__android_log_print(ANDROID_LOG_INFO, kJTag, "live view stream url is %s",streamUrl.toStdString().c_str());
    return true;
    }
