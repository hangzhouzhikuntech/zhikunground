import QtQuick                  2.5
import QtQuick.Controls         1.4
import QtQuick.Dialogs          1.2
import QtQuick.Window 2.0
import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controllers   1.0
QGCViewDialog{
//Item{
    id :gimbalAdjust;
    visible: true;
    width: 800;
    height: 400;
Rectangle{

    //color: "black";
    Row{

        anchors.fill: parent;
        spacing: 20;
        Column{

            width: 800;
            spacing: 16;



            CameraView
            {
                id : cameraImageView
                width:480
                height:360


            }
            LiveViewPaintWorker
            {
                id :paintWorker
                //parent: 0
                cameraView:cameraImageView

            }
            /*QSonyCameraView
            {
                id: cameraView
                viewLabel:cameraImageView


            }*/
            QSonyCameraController
            {
                id : sonyCameraController
                paintView:paintWorker
                //cameraView: cameraView

            }

            /*Connections {
                target: cameraView
                onNewImage: {
                    cameraImageView.setImage(img)
                }
            }*/




            ExternalDeviceComponent{
                id : externController;
            }
            Text{
                id:sliderStat;
                color: "blue";
                text:"roll : 0";


            }

            Slider{

                width: 800;
                height: 80;
                minimumValue: -80;
                maximumValue: 80;
                stepSize: 1;
                value: 0;
                onValueChanged: {
                    sliderStat.text = "roll : " + value;
                    externController.gimbalChanged(value,0);
                    //cameraView.openView()

                }

            }
            Text{
                id:pitchSlider;
                color: "blue";
                text:"pitch : 0";


            }
            Slider{

                width: 800;
                height: 80;
                minimumValue: -80;
                maximumValue: 80;
                stepSize: 1;
                value: 0;
                onValueChanged: {
                    pitchSlider.text = "pitch : " + value;
                    externController.gimbalChanged(value,1);

                }

            }

            Text{
                id:yawSlider;
                color: "blue";
                text:"yaw : 0";


            }
            Slider{

                width: 800;
                height: 80;
                minimumValue: -90;
                maximumValue: 90;
                stepSize: 1;
                value: 0;
                onValueChanged: {
                    yawSlider.text = "yaw : " + value;
                    externController.gimbalChanged(value,2);

                }

            }




        }



    }

}
}
//}
