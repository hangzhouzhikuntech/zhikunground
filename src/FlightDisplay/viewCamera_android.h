#ifndef QSONYCAMERAVIEW_H
#define QSONYCAMERAVIEW_H
#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork>
#include <QHostAddress>
#include <QQuickView>
//#include <QThread>
//#include "qmlapplicationviewer.h"
#include <QQmlApplicationEngine>
#include <QString>
#include <QQuickPaintedItem>
#include <QImage>
#include <QQueue>
#include <QMutex>
#include <android/log.h>
struct ServerDevice {
    QString mName;
    QString mActionListUrl;
    QString mDDUrl;
    QString mFriendlyName;
    QString mModelName;
    QString mUDN;
    QString mIconUrl;
};

class CameraView:public QQuickPaintedItem
{
    Q_OBJECT
public:
    CameraView();
    ~CameraView();
    void paint(QPainter *painter);
    QImage& getImageView();
    void setImage(QImage img);
private:

    QImage _image;
public slots:

};
class LiveViewPaintWorker:public QObject
{
    Q_OBJECT
    Q_PROPERTY(CameraView *cameraView READ cameraView WRITE setCameraView)
public:
    explicit LiveViewPaintWorker(QObject*parent=0);
    ~LiveViewPaintWorker();
    CameraView * _cameraView;
    void setQueue(QQueue<QByteArray> *queue);
    CameraView *cameraView(){return _cameraView;};
    void setCameraView(CameraView*view){_cameraView = view;};
    QMutex *_mutex;
private:
    QQueue<QByteArray>* _paintQueue;
    bool showFlag;
public slots:
    void startView();
    void updateView();
signals:
    void startThread();

};
class QSonyCameraView : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CameraView* viewLabel READ getLabel WRITE setLabel)
public:
    //QSonyCameraView(){};

    explicit QSonyCameraView(QObject *parent = 0);
    ~QSonyCameraView();
    CameraView* getLabel();
    void setLabel(CameraView*label);
    Q_INVOKABLE void    openView       ();
    void closeView();
     void InitSocket();
     void sendRequest();
     QNetworkAccessManager *_netWorkManager;
     //QNetworkRequest *_netWorkRequest;
     QNetworkReply* reply;
     void fetch(QString ddUrl);
     void startLiveview();
     void stopLiveview();
     bool getPayLoadHeader(QByteArray &replyArry,int *arryIndex);
     void getPayLoadData(QByteArray &replyArry,int pkgLen,int *arryIndex);
     int bytes2int(QByteArray bytes,int len);
     void setQueue(QQueue<QByteArray>* queue);
     QMutex *_mutex;
     QByteArray _payloadHeader;
private slots:
    void Recv();
    void networkTimeout();
    void networkReplyFinished();
    void networkReplyError();
    void networkReplyReadyRead();
private:
    int retryTimes;
    int _liveViewState;
    QUdpSocket * ssdpClient;
    bool receiveFlag;
    QString ddUsn;
    QString ddLocation;
    ServerDevice _device;
    unsigned int handleState;
    CameraView * _viewLabel;
    QQuickView *qmlView;
    QQmlApplicationEngine *_qmlEngine;
    int _jpegSize;
    int _recvSize;
    int _paddingSize;
    QByteArray _jpegData;
    QQueue<QByteArray> *_paintQueue;
    int headerLeftLen;
signals:
    void updateImage();
};
#endif

