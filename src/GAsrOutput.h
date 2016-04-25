/*=====================================================================

PIXHAWK Micro Air Vehicle Flying Robotics Toolkit

(c) 2009, 2010 PIXHAWK PROJECT  <http://pixhawk.ethz.ch>

This file is part of the PIXHAWK project

    PIXHAWK is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PIXHAWK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PIXHAWK. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of audio output
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#ifndef GASROUTPUT_H
#define GASROUTPUT_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QStringList>

#include "QGCAsrWorker.h"
#include "QGCToolbox.h"

class QGCApplication;

/**
 * @brief Audio Output (speech synthesizer and "beep" output)
 * This class follows the singleton design pattern
 * @see http://en.wikipedia.org/wiki/Singleton_pattern
 */
class GAsrOutput : public QGCTool
{
    Q_OBJECT

public:
    GAsrOutput(QGCApplication* app);
    ~GAsrOutput();
    void _startAsring();

public slots:
    void _stopAsring(qint32 extType);
signals:

    void startAsring();
    void stopAsring(qint32 extType);
protected:


#if !defined __android__
    QThread* thread;
    QGCAsrWorker* worker;
#endif

};

#endif // ASROUTPUT_H

