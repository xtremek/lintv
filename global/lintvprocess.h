/***************************************************************************
   This file is part of LinTV
   lintvprocess.h
   An abstraction of KProcess to be more usable by LinTV classes.
   Code taken from K9Copy source code in file k9process.h.

   Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007, revised 
      for use in LinTV by Jesse L. Zamora <xtremek2008@aim.com>

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

#ifndef LINTVPROCESS_H
#define LINTVPROCESS_H

#include <kprocess.h>

class LinTVProcess : public KProcess
{
    Q_OBJECT

public:
    LinTVProcess(QObject *parent = 0);
    
    virtual bool start();
    bool isRunning();
    bool normalExit();

signals:
    void receivedStderr(KProcess *, char *, int);
    void receivedStdout(KProcess *, char *, int );

private slots:
    void slotReadyReadStandardError();
    void slotReadyReadStandardOutput();

private:
};

#endif
