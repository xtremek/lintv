/***************************************************************************
   This file is part of LinTV
   lintvdevicectl.cpp
   Controls v4l2 devices in the system. The code here was taken from v4l2-ctl.cpp
   file in the ivtv source code.

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

#include "lintvdevicectl.h"

#include <kdebug.h>

#include <sys/ioctl.h>
#include <fcntl.h>

struct v4l2_capability LinTVDeviceCtl::vcap;
struct v4l2_input LinTVDeviceCtl::vin;
int LinTVDeviceCtl::fd;

LinTVDeviceCtl::LinTVDeviceCtl(QObject *parent)
    : QObject(parent)
{
}

LinTVDeviceCtl::~LinTVDeviceCtl()
{
}


void LinTVDeviceCtl::init(const QString & deviceStr)
{
    fd = -1;
    memset(&vcap, 0, sizeof(vcap));
    memset(&vin, 0, sizeof(vin));

    fd = open(deviceStr.toUtf8(), O_RDWR);
    doioctl(fd, VIDIOC_QUERYCAP, &vcap);
}

void LinTVDeviceCtl::setInput(const QString & device, int input)
{
    init(device);

    int in = input;

    if (doioctl(fd, VIDIOC_S_INPUT, &in) == 0) {
        vin.index = in;
        ioctl(fd, VIDIOC_ENUMINPUT, &vin);
    }
}

LinTVDevice LinTVDeviceCtl::deviceInfo(const QString & deviceStr)
{
    LinTVDevice device;

    init(deviceStr);

    device.deviceNode = deviceStr;
    device.driver = (char *)vcap.driver;
    device.deviceName = (char *)vcap.card;

    // Read the inputs
    vin.index = 0;
    while (ioctl(fd, VIDIOC_ENUMINPUT, &vin) >= 0) {
        device.deviceInputs << (char *)vin.name;
      
        vin.index++;
    }

    return device;
}

int LinTVDeviceCtl::doioctl(int fd, int request, void *parm)
{
    int retVal = ioctl(fd, request, parm);

    return retVal;
}