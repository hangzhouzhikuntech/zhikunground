#include <QSettings>
#include <QDebug>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QtScript/QtScript>
#include "QGC.h"
#include "QGCAsrWorker.h"
#include "QAudioOutput"
#include "QGCApplication.h"
#include "libs/qisr/inc/qisr.h"
#include "libs/qisr/inc/msp_cmn.h"
#include "libs/qisr/inc/msp_errors.h"


QString orders[MAX_ASR_ORDER_COUNT] = {"打开警灯","关闭警灯","滚转","俯仰","偏航"};
QGCAsrWorker::QGCAsrWorker(QObject *parent) :
    QObject(parent)
   ,replyTxt(NULL)
   ,_builtFlag(false)
{
    _apiId = "7942243";
    _apiKey = "hpZFmByGKudPH6CGQq1cKX2E";
    _apiSecretKey = "7ccba4b48ef4b01abd88215e0e89238d";
    _apiAccessToken ="24.599fe7dd3cb34db550b901e8738476fd.2592000.1462101639.282335-7942243";
    //API_record_format="wav";
    //API_record_HZ="8000";
    _apiLanguage ="zh";
    _apiRecordPath ="/home/zhitong/testAsr.pcm";//录音文件的路径
    _accessFlag = true;//标志,,第一次http回应要获得 API_access_token 值,第二次(以后)的http回应才是语音识别的返回结果,我把http回应都写到一个方法里,所以要区分一下
            // flag=1是得到API_access_token   flag=0是得到返回的文本
    //_netWorkManager = new QNetworkAccessManager(this);
    _outputFile = new QFile(_apiRecordPath);

    _asrResourcePath  = "fo|/home/zhitong/isr/common.jet"; //离线语法识别资源路径
    _grmBuildPath  = "/home/zhitong/isr/GrmBuilld"; //构建离线语法识别网络生成数据保存路径
    _grmFile = "/home/zhitong/isr/call.bnf"; //构建离线识别语法网络所用的语法文件
    _lexName = "contact"; //更新离线识别语法的contact槽（语法文件为此示例中使用的call.bnf）
    _grammerId = "";
    //_externalDevice = new ExternalDeviceComponent();
    for (int i = 0; i < MAX_ASR_ORDER_COUNT;i++)
    {
        _text2OrderMap.insert(i,orders[i]);
    }
    int ret;
    const char *login_config    = "appid = 57038395"; //登录参１数
    ret = MSPLogin(NULL, NULL, login_config); //第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
    if (MSP_SUCCESS != ret) {
        //printf("登录失败：%d\n", ret);
        qDebug()<<"login failed";
        return;
    }
    //_recordTimer.moveToThread(this);
    //connect(&_recordTimer,&QTimer::timeout,this,&QGCAsrWorker::_stopRecording);
}
void QGCAsrWorker::init()
{

    UserData asr_data;
    int ret                     = 0 ;
    char c;


    if ("" == _grammerId)
    {
        memset(&asr_data, 0, sizeof(UserData));
        //printf("构建离线识别语法网络...\n");
        qDebug()<<"build offline lex network";
        ret = _buildGrammar(&asr_data);  //第一次使用某语法进行识别，需要先构建语法网络，获取语法ID，之后使用此语法进行识别，无需再次构建
        if (MSP_SUCCESS != ret) {
            //printf("构建语法调用失败！\n");
            qDebug()<<"build grammar failed";
            return;
        }
        while (1 != asr_data.build_fini)
            QGC::SLEEP::usleep(300 * 1000);
        if (MSP_SUCCESS != asr_data.errcode)
            return;
        _grammerId = QString(asr_data.grammar_id);


    }
    //printf("离线识别语法网络构建完成，开始识别...\n");
    _startRecording();

    /*printf("更新离线语法词典...\n");
    ret = update_lexicon(&asr_data);  //当语法词典槽中的词条需要更新时，调用QISRUpdateLexicon接口完成更新
    if (MSP_SUCCESS != ret) {
        printf("更新词典调用失败！\n");
        goto exit;
    }
    while (1 != asr_data.update_fini)
        usleep(300 * 1000);
    if (MSP_SUCCESS != asr_data.errcode)
        goto exit;
    printf("更新离线语法词典完成，开始识别...\n");
    ret = run_asr(&asr_data);
    if (MSP_SUCCESS != ret) {
        printf("离线语法识别出错: %d \n", ret);
        goto exit;
    }*/




    /*if (!_externalDevice)
    {
        _externalDevice = new ExternalDeviceComponent();
    }*/
#if 0
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
#endif
    /*init record parameters*/


}

QGCAsrWorker::~QGCAsrWorker()
{
    //_netWorkManager->deleteLater();
    //if (_audio)delete _audio;
    if (_outputFile)delete _outputFile;
    //if (_externalDevice) delete _externalDevice;
    //delete _netWorkRequest;
    MSPLogout();
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
    format.setCodec("audio/pcm");
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
int QGCAsrWorker::build_grm_cb(int ecode, const char *info, void *udata)
{
    UserData *grm_data = (UserData *)udata;

    if (NULL != grm_data) {
        grm_data->build_fini = 1;
        grm_data->errcode = ecode;
    }


    if (MSP_SUCCESS == ecode && NULL != info) {
        //printf("构建语法成功！ 语法ID:%s\n", info);
        if (NULL != grm_data)
            snprintf(grm_data->grammar_id, MAX_GRAMMARID_LEN - 1, info);
    }
    else
        //printf("构建语法失败！%d\n", ecode);
        qDebug()<<"build grammar cb failed:"<<ecode;

    return 0;
}
int QGCAsrWorker::_buildGrammar(UserData *udata)
{
    QFile grm_file(_grmFile);
    unsigned int grm_cnt_len = 0;
    char grm_build_params[MAX_PARAMS_LEN]    = {NULL};
    char *fileBuf = NULL;
    int ret = 0;



    ret = grm_file.open(QIODevice::ReadWrite);
    if(false == ret) {
        qDebug()<<"grm file open failed";
        return -1;
    }

    fileBuf = new char[grm_file.size()];
    if (NULL == fileBuf)
    {
        qDebug()<<"file memery malloc failed ";
        grm_file.close();
        return -1;
    }

    /*读入文件流*/
    QDataStream in(&grm_file);
    in.readRawData(fileBuf,grm_file.size());
    grm_cnt_len = grm_file.size();
    grm_file.close();

    snprintf(grm_build_params, MAX_PARAMS_LEN - 1,
        "engine_type = local,asr_res_path = %s,sample_rate = %d,grm_build_path = %s",
        _asrResourcePath.toStdString().c_str(),
        SAMPLE_RATE_16K,
        _grmBuildPath.toStdString().c_str()
        );

    ret = QISRBuildGrammar("bnf", fileBuf, grm_cnt_len, grm_build_params, &QGCAsrWorker::build_grm_cb, udata);
    qDebug()<<"grm build params:"<<grm_build_params;
    qDebug()<<QString("QISRBuildGrammar return %1").arg(ret);

    delete fileBuf;

    return ret;
}
int QGCAsrWorker::update_lex_cb(int ecode, const char *info, void *udata)
{
    UserData *lex_data = (UserData *)udata;

    if (NULL != lex_data) {
        lex_data->update_fini = 1;
        lex_data->errcode = ecode;
    }

    if (MSP_SUCCESS == ecode)
        //printf("更新词典成功！\n");
        qDebug()<<"update lex data successful";
    else
        //printf("更新词典失败！%d\n", ecode);
        qDebug()<<"update lex data failed";

    return 0;
}

int QGCAsrWorker::update_lexicon(UserData *udata)
{
    const char *lex_content                   = "丁伟\n黄辣椒";
    unsigned int lex_cnt_len                  = strlen(lex_content);
    char update_lex_params[MAX_PARAMS_LEN]    = {NULL};

    snprintf(update_lex_params, MAX_PARAMS_LEN - 1,
        "engine_type = local, text_encoding = UTF-8, \
        asr_res_path = %s, sample_rate = %d, \
        grm_build_path = %s, grammar_list = %s, ",
        _asrResourcePath.toStdString().c_str(),
        SAMPLE_RATE_16K,
        _grmBuildPath.toStdString().c_str(),
        _grammerId.toStdString().c_str());
    return QISRUpdateLexicon(_lexName.toStdString().c_str(), lex_content, lex_cnt_len, update_lex_params, &QGCAsrWorker::update_lex_cb, udata);
}


void QGCAsrWorker::_startAsring()
{
    char asr_params[MAX_PARAMS_LEN]    = {NULL};
    const char *rec_rslt               = NULL;
    const char *session_id             = NULL;
    char *pcm_data                     = NULL;
    long pcm_count                     = 0;
    long pcm_size                      = 0;
    int last_audio                     = 0;
    int aud_stat                       = MSP_AUDIO_SAMPLE_CONTINUE;
    int ep_status                      = MSP_EP_LOOKING_FOR_SPEECH;
    int rec_status                     = MSP_REC_STATUS_INCOMPLETE;
    int rss_status                     = MSP_REC_STATUS_INCOMPLETE;

    int errcode                        = -1;
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
    pcm_size = inFile.size();
    pcm_data =new char[inFile.size()];
    in.readRawData(pcm_data,inFile.size());
    inFile.close();
#if 0
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
#endif
    //离线语法识别参数设置
    snprintf(asr_params, MAX_PARAMS_LEN - 1,
        "engine_type = local, \
        asr_res_path = %s, sample_rate = %d, \
        grm_build_path = %s, local_grammar = %s, \
        result_type = plain, result_encoding = UTF-8, ",
        _asrResourcePath.toStdString().c_str(),
        SAMPLE_RATE_16K,
        _grmBuildPath.toStdString().c_str(),
        _grammerId.toStdString().c_str()
        );
    qDebug()<<"asr params :"<<asr_params;
    session_id = QISRSessionBegin(NULL, asr_params, &errcode);
    if (NULL == session_id)
    {
        qDebug()<<"QISRSessionBegin faild "<<errcode;
        delete pcm_data;
        return;

    }
    //printf("开始识别...\n");
    qDebug()<<"start recognization";

    while (true) {
        unsigned int len = 6400;

        if (pcm_size < 12800) {
            len = pcm_size;
            last_audio = 1;
        }

        aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;

        if (0 == pcm_count)
            aud_stat = MSP_AUDIO_SAMPLE_FIRST;

        if (len <= 0)
            break;

        //printf(">");
        //fflush(stdout);
        errcode = QISRAudioWrite(session_id, (const void *)&pcm_data[pcm_count], len, aud_stat, &ep_status, &rec_status);
        if (MSP_SUCCESS != errcode)
        {
            delete pcm_data;
            return;
        }

        pcm_count += (long)len;
        pcm_size -= (long)len;

        //检测到音频结束
        if (MSP_EP_AFTER_SPEECH == ep_status)
            break;

        QGC::SLEEP::usleep(150 * 1000); //模拟人说话时间间隙
    }
    //主动点击音频结束
    QISRAudioWrite(session_id, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_status, &rec_status);

    delete pcm_data;
    pcm_data = NULL;

    //获取识别结果
    while (MSP_REC_STATUS_COMPLETE != rss_status && MSP_SUCCESS == errcode) {
        rec_rslt = QISRGetResult(session_id, &rss_status, 0, &errcode);
        QGC::SLEEP::usleep(150 * 1000);
    }
    //printf("\n识别结束：\n");
    //printf("=============================================================\n");
    if (NULL != rec_rslt)
    {
        //printf("%s\n", rec_rslt);
        qDebug()<<QString("recognize result %1").arg(rec_rslt);
        int order = _text2Order(QString(rec_rslt));
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
             case 2:
             {
                //roll
                extType = 2;
                break;

             }
            case 3:
            {
                //pitch
                extType = 3;
                break;
            }
            case 4:
            {
                //yaw
                extType = 4;
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
    else
    {
        //printf("没有识别结果！\n");
        qDebug()<<"recognize no result";
    }
    //printf("=============================================================\n");

    /*goto run_exit;

run_error:
    if (NULL != pcm_data) {
        free(pcm_data);
        pcm_data = NULL;
    }
    if (NULL != f_pcm) {
        fclose(f_pcm);
        f_pcm = NULL;
    }
run_exit:*/
    QISRSessionEnd(session_id, NULL);
    //return errcode;
    return;

}
void QGCAsrWorker::_asrTimeout()
{
    if (m_buf)
    {
        delete m_buf;
        m_buf = NULL;
    }
    //disconnect(replyTxt, &QNetworkReply::finished, this, &QGCAsrWorker::_networkReplyFinished);
    //disconnect(replyTxt, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &QGCAsrWorker::_networkReplyError);
    replyTxt->deleteLater();
    qDebug()<<"recogiztion time out";
    emit recognizeFailed();
}
void QGCAsrWorker::_recognizeFailed()
{
   _notice(QString("please retry again"),GAudioOutput::AUDIO_SEVERITY_NOTICE);
   qDebug()<<"recognized failed";
   /*notify user retry*/
   //_startRecording();
}


