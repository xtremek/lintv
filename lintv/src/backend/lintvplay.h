/***************************************************************************
   This file is part of LinTV
   lintvplay.h
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

#ifndef LINTVPLAY_H
#define LINTVPLAY_H

#include "lintvprocess.h"
#include "lintvdeviceinfo.h"

#include <QObject>
#include <QtGui>

class LinTVMixer;

class LinTVPlay : public QObject
{
    Q_OBJECT

public:
    /** Default constructor. This loads a background image
        into the widget since no device or inputs were specified. */
    LinTVPlay(QWidget *parent = 0);
    /** This loads up a filename, so it can play a file or device
        node (which is sotra like a file) */
    LinTVPlay(QWidget *parent, const QString & filename);
    /** This loads up a device type, device node, and input */
    LinTVPlay(QWidget *parent, LinTVDeviceInfo::DeviceType deviceType, 
              const QString & deviceStr, int input);

    /** Starts playing the video or device. If no device was specified,
      * this function doesn't do anything. 
      */
    void start();

    /** Stops playing the video or device. If no device was specified,
      * this function doesn't do anything. If waitForFinished is true,
      * this function blocks until the backend has exited.
      */
    void stop(bool waitForFinished = true);

    /** If the backend is currently running, this returns true. Otherwise false */
    bool isPlaying();

    /** This returns the current device channel */
    int channel() const;
    /** This returns the current device input */
    int input() const;
    /** Returns the current mplayer arguments as loaded by loadArgs() */
    QStringList mplayerArgs() const;

    /** This returns the current audio input used by the mixer */
    QString audioInput() const;

    /** Returns the current width of the widget */
    int videoWidth() const { return m_width; }
    /** Returns the current height of the widget */
    int videoHeight() const { return m_height; }

    /** Returns the current crop top area */
    int videoCropTop() const { return m_topCrop; }
    /** Returns the current crop left area */
    int videoCropLeft() const { return m_leftCrop; }
    /** Returns the current crop right area */
    int videoCropRight() const { return m_rightCrop; }
    /** Returns the current crop bottom area */
    int videoCropBottom() const { return m_bottomCrop; }

    /** Returns the currently used video standard */
    QString videoStandard() const { return m_videoStandard; }

    /** This sets the device string (or filename) to play from */
    void setDeviceStr(QString deviceStr);
    /** This sets the type of device to play from */
    void setDeviceType(LinTVDeviceInfo::DeviceType deviceType);

signals:
    /** This is emitted when the backend begins playing the device
        or file */
    void startedPlayback();

    /** This is for detecting when the user moves the mouse so the
        cursor can be shown in the event */
    void mouseMoved(QMouseEvent *event);

    /** When the backend finishes or stops playing, this is emitted */
    void finished(int , QProcess::ExitStatus);

public slots:
    /** Sets the volume of the device or file */
    void setVolume(int volume);
    /** Mutes or unmutes the volume */
    void muteVolume(bool on);
    /** Sets the current channel */
    void setChannel(int channel);
    /** Sets the current input */
    void setInput(int input);

    /** Sets the brightness of the video */
    void setBrightness(int value);
    /** Sets the constract of the video */
    void setContrast(int value);
    /** Sets the hue of the video */
    void setHue(int value);
    /** Sets the saturation of the video */
    void setSaturation(int value);

    /** Toggles whether or not the video is paused */
    void pauseVideo();

    /** When the user changes the audio mixer in the configuration,
        this should be called to apply the change immediately */
    void audioMixerChanged();

    /** When the user changes the video resolution, this is called to
      * apply the changes... 
      */
    void videoResolutionChanged();
    /** When the user changes the video crop area, this is called to
        apply the changes immediately */
    void videoCropAreaChanged();
    /** When the user changes the video standard, this is called to
        apply the changes immediately */
    void videoStandardChanged();

protected:
    void mouseMoveEvent(QMouseEvent *event) { emit mouseMoved(event); }

private slots:
    void receivedStdout(KProcess*, char*, int);
    void processFinished(int, QProcess::ExitStatus);

private:
    /** Loads the default values */
    void loadDefaultValues();
    /** Connects the signals and slots for the backend */
    void connectSlots();

    /** This is used to restart mplayer if the user changes a 
        setting that requires a restart */
    void restartMplayer();

    /** Sends a command string (cmd) to the backend */
    void sendCmd(QString cmd);

    /** Loads the mplayer arguments based on the current class
        values & settings. This is called repeatedly to 
        reload settings as the user changes them */
    void loadArgs();

private:
    LinTVProcess *m_mplayerProcess;
    LinTVMixer *m_mixer;
    QStringList m_mplayerArgs;
    QWidget *m_parent;

    LinTVDeviceInfo::DeviceType m_deviceType;
    QString m_device;

    QRect m_videoCropArea;

    bool m_restart;
    bool m_testVideoSize;

    int m_input;
    int m_channel;
    int m_width, m_height;
    int m_topCrop, m_bottomCrop;
    int m_leftCrop, m_rightCrop;

    QString m_videoStandard;
};

#endif 
