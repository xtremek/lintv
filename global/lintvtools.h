/***************************************************************************
   This file is part of LinTV
   lintvtools.h
   Some miscellaneous tools for LinTV.
   Code taken from K9Copy source code in file k9tools.h.

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

#ifndef LINTVTOOLS_H
#define LINTVTOOLS_H

#include <QString>


#include <kservice.h>

class KFileItem;

class LinTVTools
{
private:
    LinTVTools() {};
    ~LinTVTools() {};

public: 
   static bool checkProgram(QString programName, bool msg = false);
   static QString fullFilePath(QString );
   /** Returns the programs which support the mimetype of the KFileItem
       paramter. */
   static QMap<QString, KService::Ptr> serviceForName(const KFileItem &);
   static QString duration(const QTime& , const QTime &);
   static QString loadStyleSheet(const QString& , const QString & );

   //static int duration(const QTime &, const QTime &);

   /*static void clearOutput(QString name);
   static void setComboText(QComboBox *cb, const QString &text);
   static long getFreeSpace(const QString &path);
   static bool runAsRoot(const QString &command, const QString &description);*/

};

#endif