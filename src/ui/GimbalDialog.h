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

#ifndef GIMBALDIALOG_H
#define GIMBALDIALOG_H

#include <QDialog>

#include "ui_GimbalDialog.h"



class GimbalDialog : public QDialog
{
    Q_OBJECT

public:
    GimbalDialog(QWidget *parent = 0, Qt::WindowFlags flags = Qt::Sheet);
    ~GimbalDialog();

private slots:
    void on_RollSlider_valueChanged(int value);

    void on_PitchSlider_valueChanged(int value);

    void on_YawSlider_valueChanged(int value);

    void on_buttonBox_clicked(QAbstractButton *button);
    void on_RollSlider_sliderPressed();

    void on_RollSlider_sliderReleased();

    void on_RollSlider_sliderMoved(int position);

    void on_RollSpinBox_valueChanged(int arg1);

    void on_PitchSpinBox_valueChanged(int arg1);

    void on_YawSpinBox_valueChanged(int arg1);

signals:
    void gimbalChanged(int data,int data_type);
    void gimbalStartAdjust(void);
    void gimbalStopAdjust(void);

private:
    Ui::GimbalDialog* _ui;
    int _roll;
    int _pitch;
    int _yaw;
};

#endif
