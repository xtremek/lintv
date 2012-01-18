/***************************************************************************
   This file is part of LinTV
   lintvglobal.h
   A static class containing global functions that both LinTV and LinTVScheduler
   can use.

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

#ifndef LINTVGLOBAL_H
#define LINTVGLOBAL_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QSize>
#include <QDateTime>

#define STANDARD_ASPECT 4/3
#define WIDE_ASPECT 16/9

#define NR_OF_PRESETS 7

// Video Standards
#define NTSC "NTSC"
#define PAL "PAL"
#define SECAM "SECAM"

struct VideoAudioParameters {
    QString presetName;

    /* Video settings */
    int videoBitrate;
    /* Peak bitrates of 0 mean that the 
       video can only be constant */
    int peakVideoBitrate;
    int videoWidth;
    int videoHeight;
    QString videoFormat;
    float videoFramerate;

    /* Audio settings */
    int audioBitrate;
    QString audioFormat;
};

typedef QList<QSize> VideoResolutions;

struct CountryVideoStandard {
    QString countryName;
    const char * videoStandard;
};


#define NR_OF_PRESETS 7

class LinTVGlobal
{
public:

    LinTVGlobal();

    //static bool isDefaultResolution(int , int );
    static bool isDefaultResolution(QSize );
    static bool isLargeResolution(int , int );
    static bool isMediumResolution(int , int );
    static bool isSmallResolution(int , int );


    static VideoResolutions defaultVideoResolutions;
    static QSize largeVideoResolution;
    static QSize mediumVideoResolution;
    static QSize smallVideoResolution;

    static QList<int> audioBitrates;
    static QList<float> videoFramerates;
    static QStringList videoFormats;
    static VideoAudioParameters Presets[NR_OF_PRESETS];

    static CountryVideoStandard countryVideoStandards[];
};


#endif
