/***************************************************************************
   This file is part of LinTV Scheduler
   lintvprogrameditor.h
   A dialog for LinTV Scheduler which is used to add or edit a program.

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

#ifndef LINTVPROGRAMEDITOR_H
#define LINTVPROGRAMEDITOR_H

#include <kdialog.h>

#include "ui_program_editor.h"

#include "lintvdeviceinfo.h"
#include "lintvprogram.h"

class LinTVProgramEditor : public KDialog
{
    Q_OBJECT
public:
    enum Mode { AddProgram, EditProgram };

    LinTVProgramEditor(LinTVDevices devices, QWidget *parent = 0);
    LinTVProgramEditor(LinTVProgram program, LinTVDevices devices, 
					      QWidget *parent = 0);

    LinTVProgram program();

    void setProgramName(const QString &);
private slots:
    void checkDaysToRun(bool );
    void deviceChanged(int index);
    void inputChanged(const QString & );
    void startTimeChanged(const QTime &);
    void endTimeChanged(const QTime &);
    void startDateChanged(const QDate &);

    bool checkProgram();

    void selectAllDays();
    void deselectAllDays();

    void slotButtonClicked(int );

    //void endDateChanged(const QDate &);

private:
    void loadUi();
    void connectSignalsSlots();
    void loadCommon();

    void setNextRunLabel(const QDate &);
    void setRunTimeLabel(int secs);
private:
    QWidget *programWidget;

    Ui::programEditorWidget ui;

    LinTVProgram m_program;
    LinTVDevices m_devices;

    LinTVProgramEditor::Mode m_mode;
};

#endif
