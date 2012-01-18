/***************************************************************************
   This file is part of LinTV
   lintvplay.cpp
   This is the TV player widget for LinTV. It's pretty smart; it resizes to
   the correct aspect ratio, knows what type of devices to play, etc, etc...

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
 
#include "lintvplay.h"
#include "lintvsettings.h"
#include "ivtvutils.h"
#include "lintvmixer.h"
#include "lintvdevicectl.h"

#include <klocale.h>
#include <kprocess.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <QDir>

LinTVPlay::LinTVPlay(QWidget *parent)
    : m_parent(parent)
{
    loadDefaultValues();
}

LinTVPlay::LinTVPlay(QWidget *parent, const QString & filename)
    : m_parent(parent),
      m_device(filename)
{
    m_deviceType = LinTVDeviceInfo::File;
    m_mplayerProcess = new LinTVProcess(this);

    loadDefaultValues();
    connectSlots();
}

LinTVPlay::LinTVPlay(QWidget *parent, LinTVDeviceInfo::DeviceType deviceType,
                      const QString &deviceStr, int input)
    : m_parent(parent)
{
    loadDefaultValues();
    m_deviceType = deviceType;
    m_device = deviceStr;
    m_input = input;

    m_mplayerProcess = new LinTVProcess(this);

    connectSlots();
}

void LinTVPlay::loadDefaultValues()
{
    m_channel = 0;
    m_restart = false;

    m_width = LinTVSettings::videoWidth();
    m_height = LinTVSettings::videoHeight();

    m_mixer = new LinTVMixer(LinTVSettings::audioInput(), 80, this);

    m_topCrop = LinTVSettings::videoCropTop();
    m_bottomCrop = LinTVSettings::videoCropBottom();
    m_leftCrop = LinTVSettings::videoCropLeft();
    m_rightCrop = LinTVSettings::videoCropRight();

    m_videoStandard = LinTVSettings::videoStandard();
}

void LinTVPlay::connectSlots()
{
    connect(m_mplayerProcess, SIGNAL(receivedStdout(KProcess*, char*, int)),
            this, SLOT(receivedStdout(KProcess*, char*, int)));
    connect(m_mplayerProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
	    this, SLOT(processFinished(int, QProcess::ExitStatus)));
    //window()->resize(640, 480);
}

void LinTVPlay::start()
{
    loadArgs();
    m_mplayerProcess->start();

}

void LinTVPlay::stop(bool waitForFinished)
{
    sendCmd("quit");

    if (waitForFinished) {
        m_mplayerProcess->waitForFinished(10000);
            /*KMessageBox::error(0L, i18n("For some reason mplayer didn&apos;t close"                                              
                                        " correctly.\nWhen this occurs, you must" 
                                        " either close mplayer manually\n(type in"
                                        " sudo kill `pidof mplayer` in a terminal)"
                                        "\nor reboot your computer."), 
                                   i18n("Mplayer error!"));
        }*/
    }
}

void LinTVPlay::loadArgs()
{
    m_mplayerArgs.clear();

    // Load args now
    m_mplayerArgs << "mplayer";

    if (m_deviceType == LinTVDeviceInfo::File) {
        m_mplayerArgs << m_device;
    } else if (m_deviceType == LinTVDeviceInfo::TV) {
        if (m_channel <= 0 && m_input != 0) {
            m_mplayerArgs << "tv://";
        } else {
            m_mplayerArgs << QString("tv://%1").arg(m_channel);
        }

        m_mplayerArgs << "-tv";
        m_mplayerArgs << QString("device=%1:norm=%2:chanlist=%3:input=%4")
						.arg(m_device).arg(m_videoStandard).arg("us-cable")
                        .arg(m_input);
    } else if (m_deviceType == LinTVDeviceInfo::PVR) {
        m_mplayerArgs << m_device;
    }

    m_mplayerArgs << "-osdlevel";
    m_mplayerArgs << "0";

    m_mplayerArgs << "-wid";
    m_mplayerArgs << QString("%1").arg(m_parent->winId());

    m_mplayerArgs << "-slave";
    m_mplayerArgs << "-quiet";

    m_mplayerArgs << "-input";

    m_mplayerArgs << "nodefault-bindings";


    m_mplayerArgs << "-vf";

    QStringList videoFilters;

    if (m_deviceType != LinTVDeviceInfo::File && !m_testVideoSize) {
        // Calculate the crop area...

        videoFilters << QString("scale=%1:%2").arg(m_width)
                                              .arg(m_height);

        if (m_leftCrop != 0 || m_topCrop != 0 || m_rightCrop != 0 ||
            m_bottomCrop != 0) {
            int x, y;
            int width, height;

            x = m_leftCrop;
            y = m_topCrop;
            width = m_width - m_rightCrop - x;
            height = m_height - m_bottomCrop - y;

            videoFilters << QString("crop=%1:%2:%3:%4").arg(width).arg(height)
                                                       .arg(x).arg(y);
        }
    }

    videoFilters << "kerndeint=10:0:0:1:1";

    m_mplayerArgs << videoFilters.join(",");

    kDebug() << m_mplayerArgs;

    m_mplayerProcess->setProgram(m_mplayerArgs);
}

int LinTVPlay::channel() const
{
    return m_channel;
}

int LinTVPlay::input() const
{
    return m_input;
}

QStringList LinTVPlay::mplayerArgs() const
{
    return m_mplayerArgs;
}

void LinTVPlay::setDeviceStr(QString deviceStr)
{
    m_device = deviceStr;
    restartMplayer();
}

void LinTVPlay::setDeviceType(LinTVDeviceInfo::DeviceType deviceType)
{
    m_deviceType = deviceType;
    restartMplayer();
}

void LinTVPlay::setChannel(int channel)
{
    if (m_deviceType == LinTVDeviceInfo::PVR) {
        IVTV_Utils::setChannel(channel, m_device);
    } else if (m_deviceType == LinTVDeviceInfo::TV) {
        sendCmd(QString("tv_set_channel %1").arg(channel));
    }
    m_channel = channel;
}

void LinTVPlay::setInput(int input)
{
    if (m_deviceType == LinTVDeviceInfo::PVR) {
        stop();
    }


    LinTVDeviceCtl::setInput(m_device, input);

    if (m_deviceType == LinTVDeviceInfo::PVR) {
        start();
    }
    m_input = input;

    kDebug() << "Setting input...";
}

void LinTVPlay::setVolume(int volume)
{
    if (volume < 0 || volume > 100) {
        return;
    }
    if (m_deviceType == LinTVDeviceInfo::File || m_deviceType == LinTVDeviceInfo::PVR) {
        sendCmd(QString("volume %1 1").arg(volume));
    } else {
        m_mixer->setVolume(volume);
    }
}

void LinTVPlay::muteVolume(bool on)
{
    if (m_deviceType == LinTVDeviceInfo::File || m_deviceType == LinTVDeviceInfo::PVR) {
        if (on) {
            sendCmd("mute 1");
        } else {
            sendCmd("mute 0");
        }
    } else {
        if (on) {
            m_mixer->mute();
        } else {
            m_mixer->unmute();
        }
    }
}

void LinTVPlay::setBrightness(int value)
{
    sendCmd(QString("tv_set_brightness %1 1").arg(value));
}


void LinTVPlay::setContrast(int value)
{
    sendCmd(QString("tv_set_contrast %1 1").arg(value));
}

void LinTVPlay::setHue(int value)
{
    sendCmd(QString("tv_set_hue %1 1").arg(value));
}

void LinTVPlay::setSaturation(int value)
{
    sendCmd(QString("tv_set_saturation %1 1").arg(value));
}

void LinTVPlay::pauseVideo()
{
    sendCmd("pause");
}

void LinTVPlay::audioMixerChanged()
{
    m_mixer->setAudioInput(LinTVSettings::audioInput());
}

QString LinTVPlay::audioInput() const
{
    return m_mixer->audioInput();
}

void LinTVPlay::videoResolutionChanged()
{
    m_width = LinTVSettings::videoWidth();
    m_height = LinTVSettings::videoHeight();

    restartMplayer();
}

void LinTVPlay::videoCropAreaChanged()
{
    m_topCrop = LinTVSettings::videoCropTop();
    m_leftCrop = LinTVSettings::videoCropLeft();
    m_rightCrop = LinTVSettings::videoCropRight();
    m_bottomCrop = LinTVSettings::videoCropBottom();
  
    restartMplayer();
}

void LinTVPlay::videoStandardChanged()
{
    m_videoStandard = LinTVSettings::videoStandard();

    restartMplayer();
}

bool LinTVPlay::isPlaying()
{
    if( m_mplayerProcess )
      return m_mplayerProcess->isRunning() == true;
    else
      return false;
}

void LinTVPlay::restartMplayer()
{
    if (m_mplayerProcess->isRunning()) {
        m_restart = true;
        m_mplayerProcess->terminate();
    }
}

void LinTVPlay::receivedStdout(KProcess *, char * buffer, int buflen)
{
    QString buf = QString::fromLatin1(buffer, buflen);

    //kDebug() << buf;

    if (buf.contains("Starting playback")) {
        emit startedPlayback();
    }
}

void LinTVPlay::sendCmd(QString cmd)
{
    if (!m_mplayerProcess->isRunning()) {
        return;
    }

    QString command = cmd + "\n";

    m_mplayerProcess->write(command.toUtf8(), command.length());
}

void LinTVPlay::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_restart) {
        // Also make sure args are updated as well....
        loadArgs();

        // Start the process once again
        m_mplayerProcess->start();

        m_restart = false;
    }

    emit finished(exitCode, exitStatus);
    kDebug() << "Process finished!";
}

#include "lintvplay.moc"
