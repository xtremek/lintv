/***************************************************************************
   This file is part of LinTV
   lintvcolordialog.cpp
   Supplies a dialog for setting the video color settings...

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

#include "lintvcolordialog.h"

#include <klocale.h>

#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QGridLayout>

LinTVColorDialog::LinTVColorDialog(QWidget *parent, Qt::WFlags flags)
    : KDialog(parent, flags)
{
    setButtons(KDialog::Default | KDialog::Close);
    setCaption("Adjust Video Colors");

    QWidget *mainWidget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(mainWidget);

    brightnessLabel = new QLabel(i18n("Brightness:"), this);
    contrastLabel = new QLabel(i18n("Contrast:"), this);
    hueLabel = new QLabel(i18n("Hue:"), this);
    saturationLabel = new QLabel(i18n("Saturation:"), this);

    layout->addWidget(brightnessLabel, 0, 0, 1, 1);
    layout->addWidget(contrastLabel, 1, 0, 1, 1);
    layout->addWidget(hueLabel, 2, 0, 1, 1);
    layout->addWidget(saturationLabel, 3, 0, 1, 1);

    brightnessSlider = new QSlider(Qt::Horizontal, this);
    brightnessSlider->setMinimum(-100);
    brightnessSlider->setMaximum(100);
    brightnessSlider->setMinimumWidth(200);
    brightnessSlider->setTickInterval(20);
    brightnessSlider->setTickPosition(QSlider::TicksBelow);
    connect(brightnessSlider, SIGNAL(sliderMoved(int )),
            this, SLOT(brightnessSliderMoved(int )));
    connect(brightnessSlider, SIGNAL(valueChanged(int )),
            this, SLOT(brightnessSliderMoved(int )));

    contrastSlider = new QSlider(Qt::Horizontal, this);
    contrastSlider->setMinimum(-100);
    contrastSlider->setMaximum(100);
    contrastSlider->setMinimumWidth(200);
    contrastSlider->setTickInterval(20);
    contrastSlider->setTickPosition(QSlider::TicksBelow);
    connect(contrastSlider, SIGNAL(sliderMoved(int )),
            this, SLOT(contrastSliderMoved(int )));
    connect(contrastSlider, SIGNAL(valueChanged(int )),
            this, SLOT(contrastSliderMoved(int )));

    hueSlider = new QSlider(Qt::Horizontal, this);
    hueSlider->setMinimum(-100);
    hueSlider->setMaximum(100);
    hueSlider->setMinimumWidth(200);
    hueSlider->setTickInterval(20);
    hueSlider->setTickPosition(QSlider::TicksBelow);
    connect(hueSlider, SIGNAL(sliderMoved(int )),
            this, SLOT(hueSliderMoved(int )));
    connect(hueSlider, SIGNAL(valueChanged(int )),
            this, SLOT(hueSliderMoved(int )));

    saturationSlider = new QSlider(Qt::Horizontal, this);
    saturationSlider->setMinimum(-100);
    saturationSlider->setMaximum(100);
    saturationSlider->setMinimumWidth(200);
    saturationSlider->setTickInterval(20);
    saturationSlider->setTickPosition(QSlider::TicksBelow);
    connect(saturationSlider, SIGNAL(sliderMoved(int )),
            this, SLOT(saturationSliderMoved(int )));
    connect(saturationSlider, SIGNAL(valueChanged(int )),
            this, SLOT(saturationSliderMoved(int )));

    layout->addWidget(brightnessSlider, 0, 1, 1, 1);
    layout->addWidget(contrastSlider, 1, 1, 1, 1);
    layout->addWidget(hueSlider, 2, 1, 1, 1);
    layout->addWidget(saturationSlider, 3, 1, 1, 1);

    brightnessValueLabel = new QLabel("0", this);
    brightnessValueLabel->setMinimumWidth(30);
    contrastValueLabel = new QLabel("0", this);
    contrastValueLabel->setMinimumWidth(30);
    hueValueLabel = new QLabel("0", this);
    hueValueLabel->setMinimumWidth(30);
    saturationValueLabel = new QLabel("0", this);
    saturationValueLabel->setMinimumWidth(30);

    layout->addWidget(brightnessValueLabel, 0, 2, 1, 1);
    layout->addWidget(contrastValueLabel, 1, 2, 1, 1);
    layout->addWidget(hueValueLabel, 2, 2, 1, 1);
    layout->addWidget(saturationValueLabel, 3, 2, 1, 1);

    connect(this, SIGNAL(defaultClicked()),
            this, SLOT(resetValues()));
    setMainWidget(mainWidget);

}

void LinTVColorDialog::setBrightness(int value)
{
    brightnessValueLabel->setText(QString("%1").arg(value));
    brightnessSlider->setValue(value);
}

void LinTVColorDialog::setContrast(int value)
{
    contrastValueLabel->setText(QString("%1").arg(value));
    contrastSlider->setValue(value);
}

void LinTVColorDialog::setHue(int value)
{
    hueValueLabel->setText(QString("%1").arg(value));
    hueSlider->setValue(value);
}

void LinTVColorDialog::setSaturation(int value)
{
    saturationValueLabel->setText(QString("%1").arg(value));
    saturationSlider->setValue(value);
}

void LinTVColorDialog::brightnessSliderMoved(int value)
{
    brightnessValueLabel->setText(QString("%1").arg(value));
    emit brightnessChanged(value);
}

void LinTVColorDialog::contrastSliderMoved(int value)
{
    contrastValueLabel->setText(QString("%1").arg(value));
    emit contrastChanged(value);
}

void LinTVColorDialog::hueSliderMoved(int value)
{
    hueValueLabel->setText(QString("%1").arg(value));
    emit hueChanged(value);
}

void LinTVColorDialog::saturationSliderMoved(int value)
{
    saturationValueLabel->setText(QString("%1").arg(value));
    emit saturationChanged(value);
}

void LinTVColorDialog::resetValues()
{
    setBrightness(0);
    setContrast(0);
    setHue(0);
    setSaturation(0);

    emit brightnessChanged(0);
    emit contrastChanged(0);
    emit hueChanged(0);
    emit saturationChanged(0);
}