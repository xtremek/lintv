/***************************************************************************
   This file is part of LinTV
   lintvcolordialog.h
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
 
#ifndef LINTVCOLORDIALOG_H
#define LINTVCOLORDIALOG_H

#include <kdialog.h>

class QLabel;
class QSlider;
class QPushButton;

class LinTVColorDialog : public KDialog
{
    Q_OBJECT
public:
    LinTVColorDialog(QWidget *parent = 0, Qt::WFlags flags = 0);

signals:
    void brightnessChanged(int );
    void contrastChanged(int );
    void hueChanged(int );
    void saturationChanged(int );
    
public slots:
    void setBrightness(int );
    void setContrast(int );
    void setHue(int );
    void setSaturation(int );

private slots:
    void brightnessSliderMoved(int );
    void contrastSliderMoved(int );
    void hueSliderMoved(int );
    void saturationSliderMoved(int );

    void resetValues();
private:
    QLabel *brightnessLabel;
    QLabel *contrastLabel;
    QLabel *hueLabel;
    QLabel *saturationLabel;

    QSlider *brightnessSlider;
    QSlider *contrastSlider;
    QSlider *hueSlider;
    QSlider *saturationSlider;

    QLabel *brightnessValueLabel;
    QLabel *contrastValueLabel;
    QLabel *hueValueLabel;
    QLabel *saturationValueLabel;
};

#endif