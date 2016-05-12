#include "viewcamera_controller.h"
#include <android/log.h>
QSonyCameraController::QSonyCameraController()
{
    thread = new QThread();
    _cameraView = new QSonyCameraView();






}
QSonyCameraController::~QSonyCameraController()
{
    thread->quit();
    //paintThread->quit();
    delete _cameraView;
    delete _paintView;
    delete thread;
    //delete paintThread;
    //delete viewQueue;

}

void QSonyCameraController::openView()
{
    //connect(_cameraView,&QSonyCameraView::newImage,this,&QSonyCameraController::_onNewImage);
    _cameraView->setQueue(&viewQueue);
    _cameraView->_mutex = &_mutex;
    _cameraView->moveToThread(thread);
    connect(this,&QSonyCameraController::startGetWorker,_cameraView,&QSonyCameraView::InitSocket);
    thread->start();

    //paintThread = new QThread();
    _paintView->_mutex = &_mutex;
    _paintView->setQueue(&viewQueue);

    //_paintView->moveToThread(paintThread);
    //_paintView->startView();
    connect(_cameraView,&QSonyCameraView::updateImage,_paintView,&LiveViewPaintWorker::updateView);
        //paintThread->start();
    __android_log_print(ANDROID_LOG_INFO, "TEST", "QSonyCameraController construct :%d",QThread::currentThreadId());
    //_cameraView->openView();
    emit startGetWorker();
    //emit startPaintWorker();




}


void QSonyCameraController::_onNewImage(QImage img)
{
    //emit newImage(img);
}
