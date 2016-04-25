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
#ifndef EXTERNALDEVICECOMPONENT_H
#define EXTERNALDEVICECOMPONENT_H

#include <QObject>
#include <QQuickItem>

#include "UASInterface.h"
#include "AutoPilotPlugin.h"
#include "QGCLoggingCategory.h"
#include "FactPanelController.h"
#include "GimbalDialog.h"

Q_DECLARE_LOGGING_CATEGORY(ExternalDeviceComponentLog)

/// Sensors Component MVC Controller for SensorsComponent.qml.
class ExternalDeviceComponent : public FactPanelController
{
    Q_OBJECT

public:
    ExternalDeviceComponent(void);
    ~ExternalDeviceComponent();

    Q_PROPERTY(QQuickItem* statusLog MEMBER _statusLog)
    Q_INVOKABLE void _appendStatusLog(const QString& text);

    Q_INVOKABLE void controlExternalDevices(qint8 extType,qint32 *params = NULL);
    Q_INVOKABLE void _showGimbalDialog();
private slots:
    void _gimbalChanged(int data,int data_type);
    void _gimbalStartAdjust();
    void _gimbalStopAdjust();
protected:
    /*Vehicle*            _vehicle;
    UASInterface*       _uas;
    AutoPilotPlugin*    _autopilot;*/

private:
    GimbalDialog * _gimbalDialog;
    QQuickItem* _statusLog;
    };



#endif // EXTERNALDEVICECOMPONENT_H
