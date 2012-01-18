/***************************************************************************
   This file is part of LinTV
   lintvdeviceinfodialog.h
   A device info dialog for LinTV.

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
 
#ifndef LINTVDEVICEINFODIALOG_H
#define LINTVDEVICEINFODIALOG_H

#include <kdialog.h>

#include "ui_device_info.h"
#include "lintvdeviceinfo.h"

class LinTVDeviceInfoDialog : public KDialog
{
    Q_OBJECT

public:
    LinTVDeviceInfoDialog(LinTVDevices devices, QWidget *parent = 0);

private slots:
    void showHideDetails(bool );

    void selectDevice(int );

private:
    QWidget *widget;

    Ui::deviceInfoWidget ui;

    LinTVDevices m_devices;
};

#endif