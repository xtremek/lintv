/***************************************************************************
   This file is part of LinTV Scheduler
   lintvscheduler.h
   The main window widget for LinTV Scheduler. It uses the LinTVProgramEditor
   dialog for adding or editing programs.

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
 
#ifndef LINTVSCHEDULER_H
#define LINTVSCHEDULER_H

#include "lintvdeviceinfo.h"
#include "lintvprogram.h"

#include <kxmlguiwindow.h>

#include <QDate>
#include <QTime>

class KXMLGUIFactory;
class KXMLGUIBuilder;
class KToggleAction;
class KMenu;

class QTreeWidget;
class QTreeWidgetItem;
class QLabel;

class LinTVProgramHandler;
class LinTVProcess;
class LinTVProgramWidget;

class LinTVScheduler : public KXmlGuiWindow
{
    Q_OBJECT

public:
    LinTVScheduler();
    ~LinTVScheduler();

    
public slots:
    void refreshPrograms();

protected:
    void resizeEvent(QResizeEvent *);
    void contextMenuEvent(QContextMenuEvent *);

private:
    void setupActions();

    void loadPrograms();
    void updateProgram(QString, LinTVProgram );

    int detectDevices();

    void addItem(const LinTVProgram &);
    void updateItem(QTreeWidgetItem *, const LinTVProgram &);

    void updateStatusBar();

private slots:
    void addProgram();
    void editProgram();
    void removeProgram();

    void itemDoubleClick();

private:
    KXMLGUIFactory *m_guiFactory;
    KXMLGUIBuilder *m_guiBuilder;
    LinTVProgramWidget *m_programsWidget;
    KToggleAction *m_toolbarAction;

    LinTVDevices m_devicesList;
    LinTVProgramHandler* m_programHandler;

    QLabel *helpLabel;
    QLabel *m_statusLabel;

    int m_jobNumber;
};

#endif