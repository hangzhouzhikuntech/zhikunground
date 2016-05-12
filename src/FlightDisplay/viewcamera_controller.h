#ifndef VIEWCAMERA_CONTROLLER_H
#define VIEWCAMERA_CONTROLLER_H
#include <QThread>
#include <QImage>
#include <QQueue>
#include "viewCamera_android.h"
class QSonyCameraController : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(CameraView* viewLabel READ getLabel WRITE setLabel)
    Q_PROPERTY(QSonyCameraView *cameraView READ cameraView WRITE setCameraView)
    Q_PROPERTY(LiveViewPaintWorker *paintView READ paintView WRITE setPaintView)
public:
    QSonyCameraController();
    ~QSonyCameraController();
    Q_INVOKABLE void openView ();
    QSonyCameraView *cameraView(){return _cameraView;};
    void setCameraView(QSonyCameraView*view){_cameraView = view;};
    LiveViewPaintWorker *paintView(){return _paintView;};
    void setPaintView(LiveViewPaintWorker *view){_paintView = view;openView();__android_log_print(ANDROID_LOG_INFO, "TEST", "LiveViewPaintWorker setPaintView");};
private slots:
    void _onNewImage(QImage img);
private:

    QThread *thread;
    QThread *paintThread;
    QQueue<QByteArray> viewQueue;
    QMutex _mutex;
    QSonyCameraView *_cameraView ;
    LiveViewPaintWorker *_paintView;


signals:
    void startGetWorker();
    void startPaintWorker();
    void startView();
};



#endif // VIEWCAMERA_CONTROLLER_H
