/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

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

/**
 * @file
 *   @brief Implementation of audio output
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *   @author Thomas Gubler <thomasgubler@gmail.com>
 *
 */

#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "GAsrOutput.h"
#include "QGCApplication.h"
#include "QGC.h"

#if defined __android__
#include <QtAndroidExtras/QtAndroidExtras>
#include <QtAndroidExtras/QAndroidJniObject>
#endif


GAsrOutput::GAsrOutput(QGCApplication* app)
    : QGCTool(app)
#ifndef __android__
    , thread(NULL)
    , worker(NULL)
#endif
{
    thread = new QThread();
    worker = new QGCAsrWorker();
#ifndef __android__
    worker->moveToThread(thread);
    worker->_recordTimer.moveToThread(thread);
    connect(this,   &GAsrOutput::startAsring, worker, &QGCAsrWorker::init);
    connect(worker,&QGCAsrWorker::recognizeSucc,this,&GAsrOutput::_stopAsring);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    thread->start();
#endif
}

GAsrOutput::~GAsrOutput()
{
#ifndef __android__
    thread->quit();
#endif
}

void GAsrOutput::_startAsring()
{
    emit startAsring();
}

void GAsrOutput::_stopAsring(qint32 extType)
{
    qDebug()<<"GAsrOutput::_stopAsring";
    emit stopAsring(extType);
}
