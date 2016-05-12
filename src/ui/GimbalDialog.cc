/*=====================================================================

 QGroundControl Open Source Ground Control Station

 (c) 2009 - 2015 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

 This file is part of the QGROUNDCONTROL project

 QGROUNDCONTROL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 QGROUNDCONTROL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

 ======================================================================*/

#include <QSettings>
#include <QDesktopWidget>

#include "GimbalDialog.h"
//#include "MainWindow.h"
//#include "ui_SettingsDialog.h"

#include "LinkManager.h"
#include "MAVLinkProtocol.h"
#include "MAVLinkSettingsWidget.h"
#include "GAudioOutput.h"
#include "QGCApplication.h"
#include "MainToolBarController.h"
#include "FlightMapSettings.h"

GimbalDialog::GimbalDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , _ui(new Ui::GimbalDialog)
    , _roll(0)
    , _pitch(0)
    , _yaw(0)
{
    _ui->setupUi(this);

    // Center the window on the screen.
    /*QDesktopWidget *desktop = QApplication::desktop();
    int screen = desktop->screenNumber(parent);

    QRect position = frameGeometry();
    position.moveCenter(QApplication::desktop()->availableGeometry(screen).center());
    move(position.topLeft());

    MAVLinkSettingsWidget* pMavsettings  = new MAVLinkSettingsWidget(qgcApp()->toolbox()->mavlinkProtocol(), this);

    // Add the MAVLink settings pane
    _ui->tabWidget->addTab(pMavsettings,  "MAVLink");

    this->window()->setWindowTitle(tr("QGroundControl Settings"));

    _ui->tabWidget->setCurrentWidget(pMavsettings);

    connect(_ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);*/
}

GimbalDialog::~GimbalDialog()
{
    delete _ui;
}

void GimbalDialog::on_RollSlider_valueChanged(int value)
{
    _ui->RollSpinBox->setValue(value);
    qDebug()<<"value changed";
    int roll;
    int pitch;
    int yaw;
    roll = _ui->RollSpinBox->value();
    pitch = 0;
    yaw = 0;
    if (roll != _roll || pitch != _pitch || yaw != _yaw)
    {
        _roll = roll;
        _pitch = pitch;
        _yaw = yaw;
        emit gimbalChanged(roll,0);

    }
}

void GimbalDialog::on_PitchSlider_valueChanged(int value)
{
    _ui->PitchSpinBox->setValue(value);
    int roll;
    int pitch;
    int yaw;
    roll = 0;
    pitch = _ui->PitchSpinBox->value();
    yaw = 0;
    if (roll != _roll || pitch != _pitch || yaw != _yaw)
    {
        _roll = roll;
        _pitch = pitch;
        _yaw = yaw;
        emit gimbalChanged(pitch,1);

    }
}

void GimbalDialog::on_YawSlider_valueChanged(int value)
{
    _ui->YawSpinBox->setValue(value);
    int roll;
    int pitch;
    int yaw;
    roll = 0;
    pitch = 0;
    yaw = _ui->YawSpinBox->value();
    if (roll != _roll || pitch != _pitch || yaw != _yaw)
    {
        _roll = roll;
        _pitch = pitch;
        _yaw = yaw;
        emit gimbalChanged(yaw,2);

    }
}

void GimbalDialog::on_buttonBox_clicked(QAbstractButton *button)
{

}

void GimbalDialog::on_RollSlider_sliderPressed()
{
    qDebug()<<"slider pressed";
    //emit gimbalStartAdjust();

}

void GimbalDialog::on_RollSlider_sliderReleased()
{
    qDebug()<<"slider released";
    //emit gimbalStopAdjust();
}


void GimbalDialog::on_RollSlider_sliderMoved(int position)
{
    //qDebug()<<"slider moved position"<<position;



}

void GimbalDialog::on_RollSpinBox_valueChanged(int arg1)
{
    //_ui->RollSlider->setValue(arg1);
}

void GimbalDialog::on_PitchSpinBox_valueChanged(int arg1)
{
    //_ui->PitchSlider->setValue(arg1);
}

void GimbalDialog::on_YawSpinBox_valueChanged(int arg1)
{
    //_ui->YawSlider->setValue(arg1);
}

