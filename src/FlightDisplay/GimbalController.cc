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


#include "GimbalController.h"

#include "LinkManager.h"
#include "MAVLinkProtocol.h"
#include "MAVLinkSettingsWidget.h"
#include "GAudioOutput.h"
#include "QGCApplication.h"
#include "MainToolBarController.h"
#include "FlightMapSettings.h"

GimbalController::GimbalController()
{

}

GimbalController::~GimbalController()
{
}

void GimbalController::on_RollSlider_valueChanged(int value)
{
    qDebug()<<"value changed";
    if (value != _roll)
    {
        _roll = roll;
        _pitch = 0;
        _yaw = 0;
        emit gimbalChanged(roll,0);

    }
}

/*void GimbalController::on_PitchSlider_valueChanged(int value)
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

void GimbalController::on_YawSlider_valueChanged(int value)
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
}*/
