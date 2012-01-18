/***************************************************************************
   This file is part of LinTV
   lintvdevicectl.h
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

#ifndef LINTVDEVICECTL_H
#define LINTVDEVICECTL_H

#include <QObject>

#include "lintvdeviceinfo.h"

#include <linux/videodev2.h>

class LinTVDeviceCtl : public QObject
{ 
    Q_OBJECT

public:
    LinTVDeviceCtl(QObject *parent = 0);
    ~LinTVDeviceCtl();

    static void init(const QString & );
    static LinTVDevice deviceInfo(const QString & deviceStr);

    static void setInput(const QString & device, int input);

    static int doioctl(int fd, int request, void *parm);

private:
    static struct v4l2_capability vcap;
    static struct v4l2_input vin;
    static int fd;
};

#endif