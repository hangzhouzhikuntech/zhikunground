/*=====================================================================

 QGroundControl Open Source Ground Control Station

 (c) 2009, 2015 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

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

/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "ExternalDeviceComponent.h"
#include "QGCMAVLink.h"
#include "UAS.h"
#include "QGCApplication.h"

#include <QVariant>
#include <QQmlProperty>

QGC_LOGGING_CATEGORY(ExternalDeviceComponentLog, "ExternalDeviceComponentrLog")

ExternalDeviceComponent::ExternalDeviceComponent(void) :
    _statusLog(NULL)
{
    emit InitOK();
}

ExternalDeviceComponent::~ExternalDeviceComponent()
{
}
/// Appends the specified text to the status log area in the ui
void ExternalDeviceComponent::_appendStatusLog(const QString& text)
{
    Q_ASSERT(_statusLog);

    QVariant returnedValue;
    QVariant varText = text;
    QMetaObject::invokeMethod(_statusLog,
                              "append",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, varText));
}

void ExternalDeviceComponent::_gimbalChanged(int data,int data_type )
{
    qDebug()<<"gimbal data :"<<data<<",data type:"<<data_type;
    int params[7] ;
    memset(params,0,7*sizeof(int));
    params[0] = data;
    params[1] = data_type;
    Vehicle* vehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();

    if (vehicle) {
        UASInterface *uas = vehicle->uas();
        if (uas)
        {
            uas->controlExternalDevices(UASInterface::StartExternalDeviceGimbalAdjust,params);
        }
    }
    //controlExternalDevices(UASInterface::StartExternalDeviceRollGimbal,params);
}
void ExternalDeviceComponent::_gimbalStartAdjust()
{
    qDebug()<<"gimbal start adjust";
    Vehicle* vehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();

    if (vehicle) {
        UASInterface *uas = vehicle->uas();
        if (uas)
        {
            uas->controlExternalDevices(UASInterface::StartExternalDeviceGimbalStart);
        }
    }
    //controlExternalDevices(UASInterface::StartExternalDeviceRollGimbal,params);
}
void ExternalDeviceComponent::_gimbalStopAdjust()
{
    qDebug()<<"gimbal start adjust";
    Vehicle* vehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();

    if (vehicle) {
        UASInterface *uas = vehicle->uas();
        if (uas)
        {
            uas->controlExternalDevices(UASInterface::StartExternalDeviceGimbalStop);
        }
    }
    //controlExternalDevices(UASInterface::StartExternalDeviceRollGimbal,params);
}
void ExternalDeviceComponent::controlExternalDevices(qint8 extType,qint32 *params)
{
    //_startLogCalibration();
    //UASInterface::StartExternalDeviceType
    qDebug()<<"ExternalDeviceComponent::controlExternalDevices:"<<extType;
    Vehicle* vehicle = qgcApp()->toolbox()->multiVehicleManager()->activeVehicle();

    if (vehicle) {
        UASInterface *uas = vehicle->uas();
        if (uas)
        {
            uas->controlExternalDevices((UASInterface::StartExternalDeviceType)extType,params);
        }
    }
}

