/***************************************************************************
   This file is part of LinTV
   lintvdeviceinfo.cpp
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

#include "lintvdeviceinfo.h"
#include "lintvdevicectl.h"

#include <kdebug.h>

#include <QRegExp>
#include <QFile>

LinTVDeviceInfo::LinTVDeviceInfo()
{
}

LinTVDeviceInfo::~LinTVDeviceInfo()
{
}

LinTVDevices LinTVDeviceInfo::detectDevices()
{
    LinTVDevices devices;

    for (int i = 0; i < 5; i++) {
        QString deviceStr = QString("/dev/video%1").arg(i);
        QFile deviceNode(deviceStr);
        if (deviceNode.exists()) {
            devices << LinTVDeviceCtl::deviceInfo(deviceStr);
        }
    }

    return devices;
}

LinTVDeviceInfo::DeviceType LinTVDeviceInfo::deviceType(LinTVDevice device)
{
    if (device.driver == "ivtv") {
        return LinTVDeviceInfo::PVR;
    } else {
        return LinTVDeviceInfo::TV;
    }
}

#include "lintvdeviceinfo.moc"