/***************************************************************************
   This file is part of LinTV
   ivtvutils.cpp
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
 
#include "ivtvutils.h"
#include "lintvprocess.h"

IVTV_Utils::IVTV_Utils() 
{
}

void IVTV_Utils::setChannel(int channel, QString deviceNode)
{
    LinTVProcess m_ivtvProcess;
    m_ivtvProcess << "ivtv-tune";
    m_ivtvProcess << "-d";
    m_ivtvProcess << deviceNode;
    m_ivtvProcess << "-c";
    m_ivtvProcess << QString("%1").arg(channel);
    m_ivtvProcess.start();
    m_ivtvProcess.waitForFinished();
}