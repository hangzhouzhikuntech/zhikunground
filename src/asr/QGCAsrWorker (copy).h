#ifndef QGCASRWORKER_H
#define QGCASRWORKER_H

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QAudioInput>
#include <QFile>
#include "ExternalDeviceComponent.h"
class QGCAsrWorker : public QObject
{
    Q_OBJECT
public:
    explicit QGCAsrWorker(QObject *parent = 0);
    ~QGCAsrWorker();
    void init();
    void _startRecording();
    void _notice(const QString &text,int severity);
    int32_t _text2Order(const QString &text);
    void _startAsring();
    void closeFile();
    QTimer _recordTimer;
    void _asrTimeout();
signals:

    void recognizeFinished(QString text);
    void recognizeFailed();
    void recognizeSucc(qint32 extType);
protected:
    int voiceIndex;   ///< The index of the flite voice to use (awb, slt, rms)
private:
    QNetworkAccessManager *_netWorkManager;
    QNetworkRequest *_netWorkRequest;
    QString _apiAccessToken;
    QString _apiId;
    QString _apiKey;
    QString _apiSecretKey;
    QString _apiRecordPath;
    //QString  API_record_format;
   // QString API_record_HZ;
    QString _apiLanguage;
    char * m_buf;
    bool _accessFlag;
    QAudioInput *_audio;
    QFile *_outputFile;
    QNetworkReply* replyTxt;
    QMap<int32_t,QString> _text2OrderMap;

    //ExternalDeviceComponent *_externalDevice;
private slots:
    void _networkReplyFinished          ();
    void _networkReplyError             (QNetworkReply::NetworkError error);
    void _stopRecording                 ();
    void _recognizeFinished             (QString text);
    void _recognizeFailed               ();
};

#endif // QGCASRWORKER_H
