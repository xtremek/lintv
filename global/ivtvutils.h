/***************************************************************************
   This file is part of LinTV
   ivtvutils.h
   A small static utility class for using the ivtv-tune program for WinTV 
   devices.

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

#ifndef IVTVUTILS_H
#define IVTVUTILS_H

#include <QString>

class LinTVProcess;

class IVTV_Utils
{
public:
    IVTV_Utils();

    static void setChannel(int , QString );
};

#endif