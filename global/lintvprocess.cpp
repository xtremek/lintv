/***************************************************************************
   This file is part of LinTV
   lintvprocess.cpp
   An abstraction of KProcess to be more usable by LinTV classes.
   Code taken from K9Copy source code in file k9process.cpp.

   Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007, revised 
      for use in LinTV by Jesse L. Zamora <xtremek2009@gmail.com>

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

#include "lintvprocess.h"
#include "lintvtools.h"

LinTVProcess::LinTVProcess(QObject *parent)
    : KProcess(parent)
{
    setOutputChannelMode(KProcess::SeparateChannels);
    connect(this, SIGNAL(readyReadStandardError()),
	    this, SLOT(slotReadyReadStandardError()));
    connect(this, SIGNAL(readyReadStandardOutput()),
	    this, SLOT(slotReadyReadStandardOutput()));
}

bool LinTVProcess::start()
{
    QString programName;
    programName = program().at(0);

    if (!LinTVTools::checkProgram(programName)) {
	// show an error dialog
	return false;
    }

    KProcess::start();
    return (state() != QProcess::NotRunning);
}

bool LinTVProcess::isRunning()
{
    return state() == QProcess::Running;
}

bool LinTVProcess::normalExit() 
{
    return exitStatus() == QProcess::NormalExit;
}

void LinTVProcess::slotReadyReadStandardError()
{
    QByteArray ba = readAllStandardError();
    emit receivedStderr(this, ba.data(), ba.length());
}

void LinTVProcess::slotReadyReadStandardOutput() {
   QByteArray ba = readAllStandardOutput();
   emit receivedStdout(this, ba.data(), ba.length());
}

#include "lintvprocess.moc"