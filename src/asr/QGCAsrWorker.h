#ifndef QGCASRWORKER_H
#define QGCASRWORKER_H

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QAudioInput>
#include <QFile>
#define SAMPLE_RATE_16K     (16000)
#define SAMPLE_RATE_8K      (8000)
#define MAX_GRAMMARID_LEN   (32)
#define MAX_PARAMS_LEN      (1024)
#define MAX_ASR_ORDER_COUNT 5
typedef struct tagUserData {
    int     build_fini; //标识语法构建是否完成
    int     update_fini; //标识更新词典是否完成
    int     errcode; //记录语法构建或更新词典回调错误码
    char    grammar_id[MAX_GRAMMARID_LEN]; //保存语法构建返回的语法ID
}UserData;
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
    int _buildGrammar(UserData *udata);
    static int update_lex_cb(int ecode, const char *info, void *udata);
    int update_lexicon(UserData *udata); //更新离线识别语法词典
    //int run_asr(UserData *udata); //进行离线语法识别
    static int build_grm_cb(int ecode, const char *info, void *udata);
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
    QString  _asrResourcePath;//离线语法识别资源路径
    QString _grmBuildPath; //构建离线语法识别网络生成数据保存路径
    QString _grmFile; //构建离线识别语法网络所用的语法文件
    QString _lexName; //更新离线识别语法的contact槽（语法文件为此示例中使用的call.bnf）
    bool _builtFlag;
    QString _grammerId;
    /*int     _buildFini; //标识语法构建是否完成
    int     _updateFini; //标识更新词典是否完成
    int     _errcode; //记录语法构建或更新词典回调错误码
    QString    _grammarId; //保存语法构建返回的语法ID*/
    //ExternalDeviceComponent *_externalDevice;
private slots:
    void _stopRecording                 ();
    void _recognizeFailed               ();
};

#endif // QGCASRWORKER_H
