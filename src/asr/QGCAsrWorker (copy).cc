#include <QSettings>
#include <QDebug>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QtScript/QtScript>
#include "QGC.h"
#include "QGCAsrWorker.h"
#include "QAudioOutput"
#include "QGCApplication.h"
#define MAX_ASR_ORDER_COUNT 2
QString orders[MAX_ASR_ORDER_COUNT] = {"打开","关闭"};
//{"打开警灯","关闭警灯"};
QGCAsrWorker::QGCAsrWorker(QObject *parent) :
    QObject(parent)
   ,replyTxt(NULL)
{
    _apiId = "7942243";
    _apiKey = "hpZFmByGKudPH6CGQq1cKX2E";
    _apiSecretKey = "7ccba4b48ef4b01abd88215e0e89238d";
    _apiAccessToken ="24.599fe7dd3cb34db550b901e8738476fd.2592000.1462101639.282335-7942243";
    //API_record_format="wav";
    //API_record_HZ="8000";
    _apiLanguage ="zh";
    _apiRecordPath ="/home/zhitong/testAsr.wav";//录音文件的路径
    _accessFlag = true;//标志,,第一次http回应要获得 API_access_token 值,第二次(以后)的http回应才是语音识别的返回结果,我把http回应都写到一个方法里,所以要区分一下
            // flag=1是得到API_access_token   flag=0是得到返回的文本
    _netWorkManager = new QNetworkAccessManager(this);
    _outputFile = new QFile(_apiRecordPath);
    //_externalDevice = new ExternalDeviceComponent();
    for (int i = 0; i < MAX_ASR_ORDER_COUNT;i++)
    {
        _text2OrderMap.insert(i,orders[i]);
    }


    //_recordTimer.moveToThread(this);
    //connect(&_recordTimer,&QTimer::timeout,this,&QGCAsrWorker::_stopRecording);
}
void QGCAsrWorker::init()
{
    /*if (!_externalDevice)
    {
        _externalDevice = new ExternalDeviceComponent();
    }*/
    if ("" == _apiAccessToken)
    {
        QNetworkRequest req(QUrl( "https://openapi.baidu.com/oauth/2.0/token?grant_type=client_credentials&client_id=" + _apiKey + "&client_secret=" + _apiSecretKey));
        QNetworkReply* reply = _netWorkManager->get(req);
        reply->setParent(0);
        /*发送http请求,目的是得到 API_access_token口令*/
        connect(reply, &QNetworkReply::finished, this, &QGCAsrWorker::_networkReplyFinished);
        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QGCAsrWorker::_networkReplyError);
        _accessFlag = true;
    }
    else
    {
        _accessFlag = false;
        /*start recogiation*/
        _startRecording();


    }
    /*init record parameters*/


}

QGCAsrWorker::~QGCAsrWorker()
{
    _netWorkManager->deleteLater();
    //if (_audio)delete _audio;
    if (_outputFile)delete _outputFile;
    //if (_externalDevice) delete _externalDevice;
    //delete _netWorkRequest;
}
void QGCAsrWorker::closeFile()
{
    if (_outputFile->isOpen())
    {
        _outputFile->close();
    }
}
void QGCAsrWorker::_notice(const QString &text,int severity)
{
    /*if (!qgcApp()->runningUnitTests())
    {
        qgcApp()->toolbox()->audioOutput()->say(text, severity);
    }
    while(qgcApp()->toolbox()->audioOutput()->isPlaying())
    {
        QGC::SLEEP::msleep(100);
    }*/
}
int32_t QGCAsrWorker::_text2Order(const QString &text)
{
    QString OrderStr = "";
    for (QMap<int32_t,QString>::iterator it = _text2OrderMap.begin();it != _text2OrderMap.end();it++)
    {

        OrderStr = it.value();
        qDebug()<<OrderStr<<":"<<OrderStr.length();
        if ("" == OrderStr)
        {
            continue;
        }
        /*if (0 == strncmp(OrderStr.toStdString().c_str(),text.toStdString().c_str(),strlen(OrderStr.toStdString().c_str())))
        {
            return it.key();
        }*/
        if (text.contains(OrderStr))
        {
            return it.key();
        }
    }

    return 0xff;

}
void QGCAsrWorker::_startRecording()
{
    _notice(QString("Please speak out the order."), GAudioOutput::AUDIO_SEVERITY_NOTICE);
    qDebug()<<"start recording";
    /*init record parameters*/
    QAudioFormat format;
    // set up the format you want, eg.
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/wav");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
       qWarning()<<"default format not supported try to use nearest";
       format = info.nearestFormat(format);
    }
    _audio = new QAudioInput(format, this);
    if (_outputFile->isOpen())
    {
        _outputFile->close();
    }
    _outputFile->open( QIODevice::WriteOnly | QIODevice::Truncate );
    qDebug()<<"asr worker current thead id:"<<QThread::currentThreadId();
    QTimer::singleShot(3000, this, &QGCAsrWorker::_stopRecording);
    //_recordTimer.start(3000);

    if (!_audio)
    {
        qWarning("record not ready!!!");
        closeFile();
        return;
    }
    _audio->start(_outputFile);
    // Records audio for 3000ms
    qDebug() <<"record begin!";
}
void QGCAsrWorker::_stopRecording()
{
    qDebug()<<"_stopRecording";
    //_recordTimer.stop();
    _audio->stop();
    if (_audio)delete _audio;
    closeFile();
    _startAsring();

}
void QGCAsrWorker::_startAsring()
{

    qDebug()<<"start speech recogniation";
    QFile inFile(_apiRecordPath);
    if( !(inFile.open(QIODevice::ReadOnly)))
    {
        //QMessageBox::warning(this,"警告","打开语音文件失败！");
        qWarning("open audio file failed!");
        return;
    }
    /*读入文件流*/
    QDataStream in(&inFile);
    m_buf =new char[inFile.size()];
    in.readRawData(m_buf,inFile.size());


    /*发送http请求,目的是得到语音文本*/
    QString  getTextUrl = "http://vop.baidu.com/server_api?lan=" + _apiLanguage + "&cuid=" + _apiId + "&token=" + _apiAccessToken;
    QUrl url;
    url.setUrl(getTextUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "audio/wav;rate=16000");
    QByteArray arr = QByteArray(m_buf, inFile.size());
    replyTxt = _netWorkManager->post(request,arr);
    inFile.close();
    //QTimer::singleShot(30000, this, &QGCAsrWorker::_asrTimeout);
    connect(replyTxt, &QNetworkReply::finished, this, &QGCAsrWorker::_networkReplyFinished);
    connect(replyTxt, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QGCAsrWorker::_networkReplyError);
}
void QGCAsrWorker::_asrTimeout()
{
    if (m_buf)
    {
        delete m_buf;
        m_buf = NULL;
    }
    disconnect(replyTxt, &QNetworkReply::finished, this, &QGCAsrWorker::_networkReplyFinished);
    disconnect(replyTxt, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QGCAsrWorker::_networkReplyError);
    replyTxt->deleteLater();
    qDebug()<<"recogiztion time out";
    emit recognizeFailed();
}
void QGCAsrWorker::_networkReplyFinished()
{
    if (m_buf)
    {
        delete m_buf;
        m_buf = NULL;
    }
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if(!reply) {
        qWarning() << "QGCMapEngineManager::networkReplyFinished() NULL Reply";
        return;
    }
    if(true == _accessFlag)
    {
                /*QString all=reply->readAll();
                QJsonParseError jsonError;
                QJsonDocument parse_doucment = QJsonDocument::fromJson(all.toUtf8(), &jsonError);
                qDebug()<<jsonError.error<<endl;
                if(jsonError.error == QJsonParseError::NoError)
                {
                  if(parse_doucment.isObject())
                    {
                       QJsonObject jsonObj = parse_doucment.object();
                        if(jsonObj.contains("access_token"))
                            API_access_token=jsonObj.take("access_token").toString();
                    }
                 }*/
            qDebug()<<"get access token finished";

            QString strJsonAccess=reply->readAll();//得到http返回的所有信息,这个返回的JSON包所携带的是API_access_token口令
            qDebug()<<"get access token message info"<<strJsonAccess;
            /*以下是对返回信息(JSON包)进行解析*/
            QScriptValue jsonAccess;
            QScriptEngine engineAccess;
            jsonAccess = engineAccess.evaluate("value = " + strJsonAccess);
            QScriptValueIterator iteratorAccess(jsonAccess);
            while (iteratorAccess.hasNext())
            {
                    iteratorAccess.next();
                  if(iteratorAccess.name()=="access_token")
                      _apiAccessToken = iteratorAccess.value().toString();//得到 access_token,验证是百度用户
             }
            if(_apiAccessToken == "")
            {
                //QMessageBox::warning(this,"警告","access_token口令获取失败！");
                qWarning("get access token failed");
                reply->deleteLater();//释放
                return;
            }
            qDebug()<<"access token "<<_apiAccessToken;
            _accessFlag = false;
            reply->deleteLater();//释放

            /*start recogiation*/
            _startRecording();
        }
        else
        {
                /*QString strJsonText = reply->readAll();//对文本进行解析,,这个返回的JSON包所携带的是语音文本
                QString strText="";
                QScriptValue jsontext;
                QScriptEngine engineText;
                jsontext = engineText.evaluate("value = " + strJsonText );
                QScriptValueIterator iteratorText(jsontext);
                while (iteratorText.hasNext())
                {
                       iteratorText.next();
                      if(iteratorText.name()=="result")
                      {
                          strText = iteratorText.value().toString();
                          qDebug()<<"get the recognition result" <<strText;
                          break;
                      }
                 }*/

               QString strJsonText = reply->readAll();//对文本进行解析,,这个返回的JSON包所携带的是语音文本
               qDebug()<<"get recognition result finished"<<strJsonText;
               QString strText="";
               QScriptEngine engineText;
               QScriptValue jsontext = engineText.evaluate("value = " + strJsonText );
               QScriptValueIterator iteratorText(jsontext);
               while (iteratorText.hasNext())
               {
                      iteratorText.next();
                     if(iteratorText.name()=="result")
                     {
                         strText = iteratorText.value().toString();
                         qDebug()<<"get the recognition result" <<strText;
                         //_notice(QString("recognition order is %1.").arg(QString(strText)), GAudioOutput::AUDIO_SEVERITY_NOTICE);
                         if ("" != strText)
                         {
                            _recognizeFinished(QString(strText));
                         }
                         break;
                     }
                }
               if ("" == strText)
               {
                   _notice(QString("recognition failed"), GAudioOutput::AUDIO_SEVERITY_NOTICE);
                   _recognizeFailed();
               }
               reply->deleteLater();
        }

}

void QGCAsrWorker::_recognizeFinished(QString text)
{
    qDebug()<<"recognizeFinished";
   int32_t order = _text2Order(text);
   int32_t extType = 0;
   switch (order)
    {
        case 0:
        {
            qDebug()<<"open light";
            //_externalDevice->controlExternalDevices(1);
            extType = 1;
            break;
        }
        case 1:
        {
            qDebug()<<"close light";
            //_externalDevice->controlExternalDevices(0);
            extType = 0;
            break;
        }
        default:
        {
            qDebug()<<"invalid order name";
            emit recognizeFailed();
            return;
        }
    }

   emit recognizeSucc(extType);
}
void QGCAsrWorker::_recognizeFailed()
{
   _notice(QString("please retry again"),GAudioOutput::AUDIO_SEVERITY_NOTICE);
   qDebug()<<"recognized failed";
   /*notify user retry*/
   //_startRecording();
}

void QGCAsrWorker::_networkReplyError(QNetworkReply::NetworkError error)
{
    if (m_buf)
    {
        delete m_buf;
        m_buf = NULL;
    }
    qDebug()<<"Reply error";
    _recognizeFailed();
}
