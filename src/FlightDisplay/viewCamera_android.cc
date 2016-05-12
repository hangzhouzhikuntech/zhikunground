/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Copyright (C) 2012 Andre Hartmann <aha_1980@gmx.de>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

//  Written by: S. Michael Goza 2014
//  Adapted for QGC by: Gus Grubba 2015


#include <errno.h>
#include <stdio.h>

#include <QtAndroidExtras/QtAndroidExtras>
#include <QtAndroidExtras/QAndroidJniObject>

#include "viewCamera_android.h"
#include <QDomDocument>
#include <QPainter>
#include <QLabel>
#include <QtQuick>
#include <QGCApplication.h>
#include <android/log.h>
QT_BEGIN_NAMESPACE
#define DEGUG_LEVEL 0
#if DEGUG_LEVEL
#define ANDROID_DEBUG __android_log_print
#else
#define ANDROID_DEBUG
#endif
static const char kJniClassName[] {"org/qgroundcontrol/sony/DeviceDiscoveryActivity"};
static const char kJTag[] {"QGC_QSonyCameraView"};
#define SSDP_PORT 1900
#define SSDP_ADDR "239.255.255.250"
#define SSDP_MX 1
#define SSDP_ST "urn:schemas-sony-com:service:ScalarWebAPI:1"
#define SSDP_MAX_RETRY_TIMES 3

CameraView::CameraView( )
{
    setAntialiasing(true);
}
CameraView::~CameraView()
{
    //delete _image;
}

void CameraView::paint(QPainter *painter)
{

    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "CameraView paint %d.", _image.byteCount());
    painter->drawImage(0,0,_image);

}

QImage& CameraView::getImageView()
{
    return _image;
}
void CameraView::setImage(QImage img)
{
    _image = img;
    //fillColor();
    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "set image %d", QThread::currentThreadId());
    update();
}


//LiveViewPaintWorker
LiveViewPaintWorker::LiveViewPaintWorker(QObject*parent ):
            QObject(parent)
{
    setParent(0);
ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "LiveViewPaintWorker construct");
_mutex = nullptr;
_paintQueue = nullptr;
//connect(this,&LiveViewPaintWorker::startThread,this,&LiveViewPaintWorker::startView);
//_cameraView = new CameraView();
//startView();
}
LiveViewPaintWorker::~LiveViewPaintWorker()
{
    //delete _image;
    //delete _cameraView;
}
void LiveViewPaintWorker::updateView()
{
   _mutex->lock();

    if (!_paintQueue->isEmpty())
    {

        QByteArray imgArry = _paintQueue->dequeue();
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "get image print.");

        QImage img;
        img.loadFromData(imgArry);
        _cameraView->setImage(img);

    }
    _mutex->unlock();
}
void LiveViewPaintWorker::startView()
{
    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "LiveViewPaintWorker startView %d",QThread::currentThreadId());
    showFlag = true;
    while(true)
    {
        if (_mutex == nullptr || _paintQueue == nullptr)
        {
            //this->msleep(1000);
            continue;
        }
        _mutex->lock();
        if (!_paintQueue->isEmpty())
        {
            QByteArray imgArry = _paintQueue->dequeue();
            ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "get image print.");
            //if (img.isNull())continue;
            QImage img;
            img.loadFromData(imgArry);
            _cameraView->setImage(img);

             //QPixmap pixmap;
             //pixmap.loadFromData(imgArry);
            //_label->setPixmap(pixmap);

        }
        else
        {
            //QThread::msleep(20);
        }
        _mutex->unlock();

    }
}


void LiveViewPaintWorker::setQueue(QQueue<QByteArray>* queue)
{
    _paintQueue = queue;
    //emit startThread();
}

//QSonyCameraView
QSonyCameraView::QSonyCameraView(QObject *parent):
    QObject(parent)
{
    ssdpClient = new QUdpSocket();
    _netWorkManager = new QNetworkAccessManager(this);
    retryTimes = 0;
    receiveFlag = false;
    reply = NULL;
    handleState = 0;
    qmlView = new QQuickView();
    _qmlEngine = new QQmlApplicationEngine();
    //qmlView->setSource(QUrl::fromUserInput("qrc:qml/QGroundControl/FlightDisplay/GimbalDialog.qml"));
    //qmlView->show();
    //_label = label;
    _jpegSize = 0;
    _liveViewState = 0;
    headerLeftLen = 0;
    //timerId = startTimer(5000);
    //emit newImage(img);
    //InitSocket();
}

QSonyCameraView::~QSonyCameraView()
{
    closeView();
    delete ssdpClient;
    delete qmlView;
    delete _qmlEngine;
}

CameraView* QSonyCameraView::getLabel()
{
    return _viewLabel;
}
void QSonyCameraView::setLabel(CameraView*label)
{
    _viewLabel = label;
}
void QSonyCameraView::InitSocket()
{
  ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "init socket");
  ssdpClient->bind();
          ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "bind finished");
  connect(ssdpClient, SIGNAL(readyRead()),this, SLOT(Recv()));
  sendRequest();

}

void QSonyCameraView::sendRequest()
{
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "send request start");
    QString ssdpRequest("");
    ssdpRequest.sprintf("M-SEARCH * HTTP/1.1\r\nHOST: %s:%d\r\nMAN: \"ssdp:discover\"\r\nMX: %d\r\nST: %s\r\n\r\n",
                        SSDP_ADDR, SSDP_PORT,SSDP_MX,SSDP_ST);
    /*String.format("HOST: %s:%d\r\n", SSDP_ADDR, SSDP_PORT)
                    + String.format("MAN: \"ssdp:discover\"\r\n")
                    + String.format("MX: %d\r\n", SSDP_MX)
                    + String.format("ST: %s\r\n", SSDP_ST) + "\r\n";*/
    QByteArray datagram = ssdpRequest.toStdString().c_str();
    QHostAddress addr(SSDP_ADDR);
    ssdpClient->writeDatagram(datagram.data(),datagram.size(),addr,SSDP_PORT);
    QTimer::singleShot(3000, this, &QSonyCameraView::networkTimeout);
    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "send request success");
}
void QSonyCameraView::networkTimeout()
{
    if (receiveFlag)
    {
        return;
    }
    retryTimes++;
    if (retryTimes >= SSDP_MAX_RETRY_TIMES)
    {
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "connect failed");
        ssdpClient->close();
    }
    else
    {
        sendRequest();
    }
}
int QSonyCameraView::bytes2int(QByteArray bytes,int len)
{
    int rst = 0;
    for (int i = 0;i < len;i++)
    {
        rst = rst << 8;
        rst |= (bytes[i]&0x000000ff);

    }
    return rst;
}
 void QSonyCameraView::setQueue(QQueue<QByteArray>* queue)
 {
     _paintQueue = queue;
 }

#define containCommonHeader(a,i) ((a[i] == 0xFF) && ((a[i+1] == 0x12) ||(a[i+1] == 0x11 || a[i+1] == 0x01)) )
#define MIN(x,y) (((x) >= (y))?(y):(x))
bool QSonyCameraView::getPayLoadHeader(QByteArray &replyArry,int *arryIndex)
{
    if (_jpegSize > 0)
        return false;
    uint index = *arryIndex;
    int readLength = 0;

    if (headerLeftLen > 0)
    {
        _payloadHeader.append(replyArry.mid(index,headerLeftLen));
        index += headerLeftLen;

        _jpegSize = bytes2int(_payloadHeader.mid(4,3),3);
        _paddingSize = bytes2int(_payloadHeader.mid(7,1),1);
        if (_paddingSize > 0)
            ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "have padding size.%d", _paddingSize);

        //new payload come
        _jpegData.clear();
        _recvSize = 0;
        headerLeftLen = 0;
        *arryIndex = index;
        return true;
    }
    if (containCommonHeader(replyArry,index) )
    {
        readLength = 1 + 1 + 2 + 4;
        QByteArray commonHeader = replyArry.mid(index,readLength);
        if (commonHeader.isEmpty() || commonHeader.length() != readLength) {
            ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "common header is error");
            return false;
        }

        index += readLength;
        switch (commonHeader[1]) {
            case 0x12:
            {
                // This is information header for streaming.
                // skip this packet.
                ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "skip the packet information.");
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

                if (payloadHeader.isEmpty() || payloadHeader.length() != readLength) {
                    index += payloadHeader.length();
                    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "Cannot read stream for payload header.%d", payloadHeader.length());
                    *arryIndex = index;
                    if (payloadHeader.length() < readLength)
                    {
                        _payloadHeader.clear();
                        _payloadHeader.append(payloadHeader);
                        headerLeftLen = readLength - payloadHeader.length();
                    }
                    return false;
                }
                index += readLength;
                if (payloadHeader[0] != 0x24 || payloadHeader[1] != 0x35
                        || payloadHeader[2] !=  0x68
                        || payloadHeader[3] !=  0x79) {
                    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "Unexpected data format. (Start code)");
                    *arryIndex = index;
                    return false;
                }

                _jpegSize = bytes2int(payloadHeader.mid(4,3),3);
                _paddingSize = bytes2int(payloadHeader.mid(7,1),1);
                if (_paddingSize > 0)
                    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "have padding size.%d", _paddingSize);

                //new payload come
                _jpegData.clear();
                _recvSize = 0;
                *arryIndex = index;
                return true;
            }
        default:
            break;

        }//end switch
    }//end if pkg header
    return false;

}
void QSonyCameraView::getPayLoadData(QByteArray &replyArry,int pkgLen,int *arryIndex)
{
    uint index = *arryIndex;
    int readLength = 0;
    if (index >= pkgLen)
    {
        return;
    }
    readLength = MIN(_jpegSize - _recvSize,pkgLen - index);
    _jpegData.append(replyArry.mid(index,readLength));
    _recvSize += readLength;
    index += readLength;

    if (_recvSize == _jpegSize)
    {
        _mutex->lock();
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "it is pay load context packet,need show the view:%d,%d",_jpegData.size(),_jpegSize);
        _paintQueue->enqueue(_jpegData);
        _mutex->unlock();
        emit updateImage();
        _recvSize = 0;
        _jpegSize = 0;
    }

    *arryIndex = index;

}
 void QSonyCameraView::networkReplyReadyRead()
{
    QByteArray replyArry=reply->readAll();

    if (replyArry.isEmpty())return;
    int pkgLen = replyArry.count();
    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "networkReplyReadyRead.%d,%d",QThread::currentThreadId(),pkgLen);
    int index = 0;
    while(index < pkgLen)
    {
        //contains payload header
        getPayLoadHeader(replyArry,&index);
        getPayLoadData(replyArry,pkgLen,&index);

    }

        /*if (getPayLoad)
        {
            break;
        }*/


}

void  QSonyCameraView::networkReplyFinished()
{
    QNetworkReply* replydd = qobject_cast<QNetworkReply*>(QObject::sender());
    if(!reply) {
        //qWarning() << "QGCMapEngineManager::networkReplyFinished() NULL Reply";
        return;
    }
    QByteArray replyStr=replydd->readAll();
    if (replyStr == "")
    {
         ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"get dd infomation failed");
        return;
    }
     //ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "get access token message info %s",strJsonAccess.toStdString().c_str());

    if (handleState == 0)
    {
        QDomDocument doc;
        doc.setContent(replyStr);

        _device.mDDUrl = ddLocation;


        QDomElement docElem = doc.documentElement(); //返回根元素
        QDomNode node = docElem.firstChild(); //返回根节点的第一个子节点
        bool findFlag = false;
        while(!node.isNull())
        { //如果节点不为空
            if (node.isElement()) //如果节点是元素
            {
                //QDomElement elem = node.toElement();
                //ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"tag name %s",elem.tagName().toStdString().c_str());
                if (node.nodeName() == "device")
                {
                    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"find device");
                    QDomNodeList list = node.childNodes(); //获得元素e的所有子节点的列表
                    for(int i=0; i<list.count(); i++) //遍历该列表
                    {
                        QDomNode deviceNode = list.at(i);
                        if(deviceNode.isElement())
                        {
                            //qDebug() << “ “<< qPrintable(node.toElement().tagName())<<qPrintable(node.toElement().text());
                            ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"node name %s",deviceNode.nodeName().toStdString().c_str());
                            if (deviceNode.nodeName() == "friendlyName")
                            {
                                _device.mFriendlyName = deviceNode.toElement().text();
                            }
                            if (deviceNode.nodeName() == "modelName")
                            {
                                _device.mModelName = deviceNode.toElement().text();
                            }
                            if (deviceNode.nodeName() == "UDN")
                            {
                                _device.mUDN = deviceNode.toElement().text();
                            }
                            if (deviceNode.nodeName() == "av:X_ScalarWebAPI_DeviceInfo")
                            {

                                QDomNodeList devNodeList = deviceNode.childNodes();
                                for (int j=0;j< devNodeList.count();j++)
                                {
                                    QDomNode devNode = devNodeList.at(j);
                                    if ( devNode.nodeName() == "av:X_ScalarWebAPI_ServiceList" )
                                    {
                                        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"find web api service list");
                                        QDomNodeList serNodeList = devNode.childNodes();
                                        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"find web api service count %d",serNodeList.count());
                                        for (int k = 0;k<serNodeList.count();k++)
                                        {
                                            QDomNode serNode = serNodeList.at(k);
                                            QDomNodeList serAttrNodeList = serNode.childNodes();
                                            for (int l = 0;l<serAttrNodeList.count();l++)
                                            {
                                                ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"find web api service attr count %d",serAttrNodeList.count());
                                                QDomNode serAttrNode = serAttrNodeList.at(l);
                                                if (serAttrNode.nodeName() == "av:X_ScalarWebAPI_ServiceType")
                                                {

                                                    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"find web api service type %s",serAttrNode.toElement().text().toStdString().c_str());
                                                    if (serAttrNode.toElement().text() == "camera")
                                                    {
                                                        findFlag = true;
                                                    }
                                                }

                                                if (serAttrNode.nodeName() == "av:X_ScalarWebAPI_ActionList_URL")
                                                {
                                                    if (findFlag)
                                                    {
                                                        _device.mActionListUrl = serAttrNode.toElement().text();
                                                        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"find  ActionListUrl %s",_device.mActionListUrl.toStdString().c_str());
                                                        startLiveview();
                                                        replydd->deleteLater();
                                                        return;
                                                    }
                                                }
                                            }


                                        }
                                    }
                                }
                            }
                        }
                    }//end for
                }//end if
            }//end if

            node = node.nextSibling(); //下一个兄弟节点

        }//end while

    }
    else if (handleState == 1)
    {
        //QJsonObject replyJson;
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"json rely is %s",replyStr.toStdString().c_str());
        QJsonParseError json_error;
            QJsonDocument parse_doucment = QJsonDocument::fromJson(replyStr, &json_error);
            if(json_error.error != QJsonParseError::NoError)
            {
                return;
            }

            if(parse_doucment.isObject())
            {
                QJsonObject obj = parse_doucment.object();
                //QVariantMap result = parse_doucment.toVariant().toMap();
                QString liveviewUrl;
                if (obj.contains("result"))
                {
                    liveviewUrl = obj["result"].toArray().takeAt(0).toString();
                    ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"liveviewUrl is %s",liveviewUrl.toStdString().c_str());
                }
                if (obj.contains("id"))
                {
                    int jsonId = obj["id"].toInt();
                    //ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"id is %d",jsonId);

                }

                /*get stream*/
                QUrl url;
                url.setUrl(liveviewUrl);
                QNetworkRequest req(url);
                reply = _netWorkManager->get(req);
                //reply->setParent(0);
                bool connRest = false;
               // connect(reply, &QNetworkReply::finished, this, &QGCCameraSurfaceView::networkReplyFinished);
                connRest = connect(reply, &QNetworkReply::readyRead, this, &QSonyCameraView::networkReplyReadyRead);
                if (!connRest)
                {
                     ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "connect slot failed");
                }
                //connect(reply, &QNetworkReply::downloadProgress, this, &QGCCameraSurfaceView::networkReplyDownloadProgress);
                connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QSonyCameraView::networkReplyError);
                /*QGCCameraSurfaceView surfaceView;
                surfaceView.start(liveviewUrl);*/
                //ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"liveviewUrl is %s",liveviewUrl.toStdString().c_str());
            }
    }
    else if (handleState == 2)
    {
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"stop live view");
    }
    else
    {
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"invalid handle state");
    }

    replydd->deleteLater();
}

void  QSonyCameraView::networkReplyError()
{

}
void QSonyCameraView::fetch(QString ddUrl)
{
    if (ddUrl == NULL) {
        return;
    }

  ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "fetch url is %s",ddUrl.toStdString().c_str());
    QUrl url;
    url.setUrl(ddUrl);
    QNetworkRequest req(url);
    reply = _netWorkManager->get(req);
    reply->setParent(0);
    connect(reply, &QNetworkReply::finished, this, &QSonyCameraView::networkReplyFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QSonyCameraView::networkReplyError);
}

void QSonyCameraView::startLiveview(){
        static int requstId = 0;
        requstId++;
        QJsonObject requestJson;
        QJsonArray requstArry;
        QJsonValue requstID(requstId);
        requestJson.insert("method", "startLiveview");
        requestJson.insert("params", requstArry);
        requestJson.insert("id",requstID);
        requestJson.insert("version", "1.0");
        QJsonDocument document;
        document.setObject(requestJson);
        //QByteArray postArry = {"{\"method\":\"startLiveview\",\"params\":[],\"id\":7,\"version\":\"1.0\"}"};
        QByteArray postArry = document.toJson(QJsonDocument::Indented);
        QString url = _device.mActionListUrl + "/camera";
        QUrl postUrl;
        postUrl.setUrl(url);
        QNetworkRequest req(postUrl);
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,"startLiveview url is %s,post context is %s",url.toStdString().c_str(),postArry.toStdString().c_str());
        reply = _netWorkManager->post(req,postArry);
        handleState = 1;
        connect(reply, &QNetworkReply::finished, this, &QSonyCameraView::networkReplyFinished);
        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QSonyCameraView::networkReplyError);

}

void QSonyCameraView::stopLiveview(){
        static int requstId = 0;
        QJsonObject requestJson;
        QJsonArray requstArry;
        QJsonValue requstID(requstId);
        requestJson.insert("method", "stopLiveview");
        requestJson.insert("params", requstArry);
        requestJson.insert("id",requstID);
        requestJson.insert("version", "1.0");
        QJsonDocument document;
        document.setObject(requestJson);
        QByteArray postArry = document.toJson(QJsonDocument::Compact);
        QString url = _device.mActionListUrl + "/camera";
        QUrl postUrl;
        postUrl.setUrl(url);
        QNetworkRequest req(postUrl);
        reply = _netWorkManager->post(req,postArry);
        handleState = 2;
        connect(reply, &QNetworkReply::finished, this, &QSonyCameraView::networkReplyFinished);
        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QSonyCameraView::networkReplyError);

}
void QSonyCameraView::Recv()
{

ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "enter recv");
    QByteArray datagram;
    while (ssdpClient->hasPendingDatagrams())
    {

        datagram.resize(ssdpClient->pendingDatagramSize());
        ssdpClient->readDatagram(datagram.data(), datagram.size());
        ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "recv data:");
        /*for (int i = 0; i < datagram.size(); ++i)
        {
            ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,  " %x",datagram.at(i));
        }*/

        QString ssdpMessage(datagram.toStdString().c_str());

        int start = ssdpMessage.indexOf("USN:");
        int end = ssdpMessage.indexOf("\r\n", start);
        if (start != -1 && end != -1) {
            start += 4;
            QString val = ssdpMessage.mid(start, end);
            if (val != NULL) {
                receiveFlag = true;
                ddUsn = val.trimmed();
                start = ssdpMessage.indexOf("LOCATION:");
                end = ssdpMessage.indexOf("\r\n", start);
                if (start != -1 && end != -1) {
                    start += 9;
                    QString valLocation = ssdpMessage.mid(start, end-start+1);
                    if (NULL != valLocation)
                    {
                        ddLocation = valLocation.trimmed();
                    }
                }
                 ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag,  "ddUsn is %s,ddLocation is %s",ddUsn.toStdString().c_str(),ddLocation.toStdString().c_str());
                fetch(ddLocation);
                break;
            }
        }
    }


    /*QAndroidJniEnvironment jniEnv;;
    if (jniEnv->ExceptionCheck()) {
        jniEnv->ExceptionDescribe();
        jniEnv->ExceptionClear();
    }


    cleanSonyJavaException();

    QAndroidJniObject intent("android/content/Intent");
    intent.callObjectMethod(
                    "putExtra",
                    "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;",
                    ddUsn.object<jstring>(),
                    ddLocation.object<jstring>());
 cleanSonyJavaException();
    QtAndroid::startActivity(intent, 0);

    //QAndroidJniObject obj(sonyCameraClass,"(L/org/qtproject/qt5/android/bindings/QtActivity)V",activity.object());
//QAndroidJniObject obj(sonyCameraClass);
    ANDROID_DEBUG(ANDROID_LOG_ERROR, kJTag, "object init succ");
    //jmethodID methodViewId = jniEnv->GetMethodID(sonyCameraClass, "searchDevices", "()V");
    //QAndroidJniObject jnameL = QAndroidJniObject::fromString(systemLocation);
    cleanSonyJavaException();

  //  obj.callMethod<void>("openView");

    cleanSonyJavaException();

    //ANDROID_DEBUG(ANDROID_LOG_INFO, kJTag, "Native Functions Registered");

    if (jniEnv->ExceptionCheck()) {
        jniEnv->ExceptionDescribe();
        jniEnv->ExceptionClear();
    }*/

}
void QSonyCameraView::openView(void)
{
    //_qmlEngine = new QQmlApplicationEngine();

    //_qmlEngine->addImportPath("qrc:/qml");
   // _qmlEngine->rootContext()->setContextProperty("multiVehicleManager", toolbox()->multiVehicleManager());
    //_qmlEngine->rootContext()->setContextProperty("joystickManager", toolbox()->joystickManager());
    //_qmlEngine->load(QUrl(QStringLiteral("qrc:/qml/QGroundControl/FlightDisplay/GimbalDialog.qml")));

    //qmlView->setSource(QUrl::fromUserInput("qrc:qml/QGroundControl/FlightDisplay/GimbalDialog.qml"));
    //qmlView->show();

    InitSocket();


}
void QSonyCameraView::closeView(void)
{

    if (1 == _liveViewState)
    {
        stopLiveview();
    }
    ssdpClient->close();


}


QT_END_NAMESPACE


