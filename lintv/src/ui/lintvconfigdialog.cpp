/***************************************************************************
   This file is part of LinTV
   lintvconfigdialog.cpp
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

#include "lintvconfigdialog.h"
#include "lintvpages.h"
#include "lintvglobal.h"

LinTVConfigDialog::LinTVConfigDialog(QWidget *parent)
    : KPageDialog(parent)
{
    //LinTVGlobal::LinTVGlobal();
    setFaceType(KPageDialog::List);
    setWindowTitle(KDialog::makeStandardCaption(i18n("Settings")));

    snapshotSettingsPage = new SnapshotSettingsPage(this);
    KPageWidgetItem *page = addPage(snapshotSettingsPage, i18n("Snapshots"));
    page->setHeader(i18n("Snapshot Settings"));
    page->setIcon(KIcon("view-preview"));

    recordSettingsPage = new RecordSettingsPage(this);
    page = addPage(recordSettingsPage, i18n("Record"));
    page->setHeader(i18n("Video and Audio Recording Parameters"));
    page->setIcon(KIcon("record"));

    audioSettingsPage = new AudioSettingsPage(this);
    page = addPage(audioSettingsPage, i18n("Audio"));
    page->setHeader(i18n("Audio Mixer Settings"));
    page->setIcon(KIcon("audio-x-generic"));

    videoSettingsPage = new VideoSettingsPage(this);
    page = addPage(videoSettingsPage, i18n("Video"));
    page->setHeader(i18n("Video Standard and Cropping Settings"));
    page->setIcon(KIcon("video-x-generic"));

    connect(this, SIGNAL(okClicked()), SLOT(okClicked()));

    resize(600, 360);
}

void LinTVConfigDialog::okClicked()
{
    recordSettingsPage->acceptSettings();
    snapshotSettingsPage->acceptSettings();
    audioSettingsPage->acceptSettings();
    videoSettingsPage->acceptSettings();
    emit settingsChanged();
}

#include "lintvconfigdialog.moc"