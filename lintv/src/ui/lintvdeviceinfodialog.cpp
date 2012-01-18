/***************************************************************************
   This file is part of LinTV
   lintvdeviceinfodialog.cpp
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

#include "lintvdeviceinfodialog.h"

#include <kiconloader.h>

LinTVDeviceInfoDialog::LinTVDeviceInfoDialog(LinTVDevices devices, QWidget *parent)
    : KDialog(parent)
{
    widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);

    setCaption(i18n("Device Information"));
    setButtons(KDialog::None);
  
    KIconLoader iconLoader;
    ui.imageLabel->setPixmap(iconLoader.loadIcon("audio-card", KIconLoader::Dialog, 64));

    ui.detailToolButton->setCheckable(true);
    ui.detailToolButton->setChecked(true);

    showHideDetails(true);

    m_devices = devices;
    for (int i = 0; i < devices.size(); i++) {
        ui.devicesComboBox->addItem(devices[i].deviceName);
    }

    selectDevice(0);

    connect(ui.detailToolButton, SIGNAL(toggled(bool )),
            this, SLOT(showHideDetails(bool )));
    connect(ui.donePushButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(ui.devicesComboBox, SIGNAL(currentIndexChanged(int )),
            this, SLOT(selectDevice(int )));
}

void LinTVDeviceInfoDialog::showHideDetails(bool show)
{
    if (show) {
        ui.detailTextEdit->show();
        ui.detailToolButton->setText(i18n("Details <<"));
        setMaximumSize(480, 440);
        resize(380, 320);
    } else {
        ui.detailTextEdit->hide();
        ui.detailToolButton->setText(i18n("Details >>"));
        setMaximumSize(380, 120);
    }
}

void LinTVDeviceInfoDialog::selectDevice(int index)
{
    ui.driverLineEdit->setText(m_devices[index].driver);
    ui.deviceNodeLineEdit->setText(m_devices[index].deviceNode);
}

#include "lintvdeviceinfodialog.h"