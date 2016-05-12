#ifndef QGCCAMERASURFACEVIEW_H
#define QGCCAMERASURFACEVIEW_H
#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork>
//#include <QThread>
#include <QString>
class QGCCameraSurfaceView : public QObject
{
    Q_OBJECT
public:
    QGCCameraSurfaceView();
    ~QGCCameraSurfaceView();
    //Q_INVOKABLE
        bool    start(QString streamUrl);
     QNetworkAccessManager *_netWorkManager;
     //QNetworkRequest *_netWorkRequest;
     QNetworkReply* reply;
     QTcpSocket *tcpSocket;

private slots:
    void networkTimeout();
    void networkReplyFinished();
    void networkReplyError();
    void networkReplyDownloadProgress(qint64 byteReceived,qint64 byteTotal);

    void networkReplyReadyRead();
    void tcpDataReceive();

private:
    bool mWhileFetching;
};
#endif

