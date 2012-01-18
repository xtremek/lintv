/***************************************************************************
   This file is part of LinTV
   lintvconfigdialog.h
   Uses the pages from lintvpages.h to create the LinTV Configure dialog.

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

#ifndef LINTVCONFIGDIALOG_H
#define LINTVCONFIGDIALOG_H

#include <kpagedialog.h>
#include <kiconloader.h>
#include <kglobal.h>

class RecordSettingsPage;
class SnapshotSettingsPage;
class AudioSettingsPage;
class VideoSettingsPage;

class LinTVConfigDialog : public KPageDialog
{
    Q_OBJECT

public:
    LinTVConfigDialog(QWidget *parent);
    
signals:
    void settingsChanged();

private slots:
    void okClicked();

private:
    RecordSettingsPage *recordSettingsPage;
    SnapshotSettingsPage *snapshotSettingsPage;
    AudioSettingsPage *audioSettingsPage;
    VideoSettingsPage *videoSettingsPage;
}; 

#endif