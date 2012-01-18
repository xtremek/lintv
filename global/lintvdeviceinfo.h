/***************************************************************************
   This file is part of LinTV
   lintvdeviceinfo.h
   Gets info about system devices using LinTVDeviceCtl.

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

#ifndef LINTVDEVICEINFO_H
#define LINTVDEVICEINFO_H

#include <kprocess.h>

#include <QObject>

/** A small class designed to encapsulate & represent a
  * device or devices in the system at runtime. 
  */
class LinTVDevice 
{
public:
    /** The device node (e.g. /dev/video0) */
    QString deviceNode;
    /** The driver for the device (such as "ivtv" or "saa7134")*/
    QString driver;
    /** The name of the device */
    QString deviceName;
    /** A list of the names of the inputs the device has */
    QStringList deviceInputs;
};

typedef QList<LinTVDevice> LinTVDevices;

/**
 * A helper class that uses LinTVDeviceCtl to get info about v4l
 * devices in the system.
 *
 * @short LinTV device info class
 * @author Jesse L. Zamora <xtremek2009@aim.com>
 * @version 0.1
 */
class LinTVDeviceInfo : public QObject {
    Q_OBJECT

public:
    /** LinTV classes use this enum to describe the type of device or
      * file being played or recorded.
      */
    enum DeviceType { File, TV, PVR };

    LinTVDeviceInfo();
    ~LinTVDeviceInfo();

    /** Detects & returns a list of all v4l2 devices in the system. */
    static LinTVDevices detectDevices();

    /** A helper function to determine whether or not device is a PVR
      * device. If device uses the ivtv driver, it is a PVR device. Otherwise,
      * it's just a TV device.
      */
    static DeviceType deviceType(LinTVDevice device);
};

#endif