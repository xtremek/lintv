/***************************************************************************
   This file is part of LinTV
   lintvrecord.h
   This is the mencoder interface for LinTV to record TV.

   Author: Jesse L. Zamora <xtremek2009@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef LINTVRECORD_H
#define LINTVRECORD_H

#include "lintvdeviceinfo.h"

#include <QWidget>

class LinTVProcess;

class LinTVRecord : public QObject
{
    Q_OBJECT
public:
    LinTVRecord(const QString & filename);
    LinTVRecord(LinTVDeviceInfo::DeviceType deviceType, 
                const QString & deviceStr, int input);

    int recordTime() const { return m_minutes; }
    void setRecordTime(int minutes) { m_minutes = minutes; }


    QString recordDirectory() const { return m_recordDir; }
    void setRecordDirectory(QString dir);

    QString filename() const { return m_filename; }
    void setFilename(QString );

    int input() const { return m_input; }

    int channel() const { return m_channel; }
    void setChannel(int channel);
 
    bool doneRecording() { return m_done; }

    void start();

    bool isRecording();

    void setDeviceStr(QString deviceStr);
    void setDeviceType(LinTVDeviceInfo::DeviceType deviceType);

signals:
    void startedRecording();
    void stoppedRecording();

    void finished(int , QProcess::ExitStatus);

protected:
    void loadDefaultValues();
    void loadDefaultFilename();
    void loadArgs();
    QString videoMimeType();

protected:
    int m_input;
    int m_channel;

public slots:
    void setInput(int input);

    void stop();

private slots:
    void receivedStdout(KProcess *, char *, int);
    void processExited(int, QProcess::ExitStatus);

private:
    void connectSignalsSlots();

private:
    QWidget *displayWidget;
    QStringList m_mencoderArgs;
    LinTVProcess *m_mencoderProcess;
    
    QString m_deviceStr;
    QString m_filename;
    QString m_recordDir;

    LinTVDeviceInfo::DeviceType m_deviceType;

    bool m_started;
    bool m_done;
    int m_minutes;

    QTimer *m_recordTimer;
};

#endif 
