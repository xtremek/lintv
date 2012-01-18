/***************************************************************************
   This file is part of LinTV Scheduler
   lintvscheduler.cpp
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

#include "lintvscheduler.h"
#include "lintvschedulersettings.h"
#include "lintvprogrameditor.h"
#include "lintvglobal.h"
#include "lintvdeviceinfo.h"
#include "lintvprocess.h"
#include "lintvprogram.h"
#include "lintvtools.h"
#include "lintvprogramwidget.h"

#include "lintvscheduleradaptor.h"

#include <kapplication.h>
#include <kconfigdialog.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kxmlguibuilder.h>
#include <kxmlguifactory.h>
#include <kmenu.h>

#include <KDE/KLocale>

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QContextMenuEvent>
#include <QLabel>
#include <QBrush>

const QColor recordingItemColor(255, 191, 191);

const char * dateTimeFormat = "MM'/'dd'/'yy  h':'mm A";
LinTVScheduler::LinTVScheduler()
    : KXmlGuiWindow()
{
    setAcceptDrops(false);

    new LinTVSchedulerAdaptor( this );
    QDBusConnection::sessionBus().registerObject( "/LinTVScheduler" , this );

    m_programsWidget = new LinTVProgramWidget(this);
    setCentralWidget(m_programsWidget);

    connect(m_programsWidget, SIGNAL(itemDoubleClick()),
            this, SLOT(itemDoubleClick()));

    helpLabel = new QLabel(m_programsWidget);
    helpLabel->setAlignment(Qt::AlignCenter);
    helpLabel->resize(m_programsWidget->size());
    helpLabel->move(m_programsWidget->pos());
    helpLabel->setText(i18n("Click \"Add Program\" to\ncreate a new record schedule."));

    setupActions();
    statusBar()->show();
    setupGUI();

    m_statusLabel = new QLabel(this);
    statusBar()->addWidget(m_statusLabel);
    updateStatusBar();

    /*setXMLFile(componentData().componentName() + "ui.rc");

    m_guiBuilder = new KXMLGUIBuilder(this);
    m_guiFactory = new KXMLGUIFactory(m_guiBuilder, this);
    m_guiFactory->addClient(this);*/

    m_programHandler = new LinTVProgramHandler;

    loadPrograms();

    detectDevices();
}

LinTVScheduler::~LinTVScheduler()
{
    delete m_programsWidget;
    delete m_programHandler;
}

void LinTVScheduler::resizeEvent(QResizeEvent *)
{
    helpLabel->resize(m_programsWidget->size());
}

void LinTVScheduler::itemDoubleClick()
{
    editProgram();
}

void LinTVScheduler::setupActions()
{
    KAction *action = new KAction(KIcon("list-add"),
                                   i18n("Add Program"),
                                   this);
    actionCollection()->addAction("add_program", action);

    connect(action, SIGNAL(triggered()),
            this, SLOT(addProgram()));
    
    action = new KAction(KIcon("edit-rename"),
			 i18n("Edit Program"),
			  this);
    connect(action, SIGNAL(triggered()),
	    this, SLOT(editProgram()));
    actionCollection()->addAction("edit_program", action);

    action = new KAction(KIcon("list-remove"),
                         i18n("Remove Program"),
                         this);
    actionCollection()->addAction("remove_program", action);
    connect(action, SIGNAL(triggered()),
            this, SLOT(removeProgram()));

    action = new KAction(KIcon("view-refresh"),
                         i18n("Refresh Programs"),
                         this);
    actionCollection()->addAction("refresh_programs", action);
    connect(action, SIGNAL(triggered()),
            this, SLOT(refreshPrograms()));

    KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());

}

void LinTVScheduler::addItem(const LinTVProgram &program)
{
    QTreeWidgetItem *item  = new QTreeWidgetItem(m_programsWidget);
    item->setText(0, QString("%1").arg(program.name));
    item->setText(1, LinTVProgramHandler::nextRunTime(program).toString(dateTimeFormat));
    item->setText(2, QString("%1 ").arg(LinTVTools::duration(program.startDateTime.time(), 
                                                                program.endDateTime.time())));

    if (program.status == LinTVProgram::Queued) {
        item->setText(3, i18n("Queued"));
    } else if (program.status == LinTVProgram::Recording) {
        item->setText(3, i18n("Recording..."));
        m_programsWidget->setItemColor(item, recordingItemColor);
    }

    //item->setText(3, program.endDateTime.toString(dateTimeFormat));
    if (program.input == 0) {
        item->setText(4, i18n("%1 (Channel %2)", program.device, program.channel));
    } else {
        item->setText(4, i18n("%1 (Input %2)", program.device, program.input));
    }

    

    m_programsWidget->addTopLevelItem(item);
}

void LinTVScheduler::updateItem(QTreeWidgetItem *item, const LinTVProgram & program)
{
    // load the LinTVProgramEditor with the LinTVProgram to edit...
    int duration = program.startDateTime.time().secsTo(program.endDateTime.time()) / 60;
    item->setText(0, program.name);
    item->setText(1, program.startDateTime.toString(dateTimeFormat));
    item->setText(2, QString("%1 min").arg(duration));
    item->setText(3, program.endDateTime.toString(dateTimeFormat));
    if (program.input == 0) {
        item->setText(4, i18n("%1 (Channel %2)", program.device, program.channel));
    } else {
        item->setText(4, i18n("%1 (Input %2)", program.device, program.input));
    }
}

void LinTVScheduler::updateStatusBar()
{
    /*LinTVProgram program = LinTVProgramHandler::nextRunProgram();
    QDateTime dateTime = program.startDateTime;

    if (dateTime != QDateTime()) {
        if (program.status == LinTVProgram::Recording) {
            m_statusLabel->setText(i18n("<i>Currently Recording: %1</i>", program.name));
        } else {
            m_statusLabel->setText(i18n("<i>Next Run Date: %1 at %2</i>", dateTime.date().toString(LinTVProgramHandler::dateFormat()),
                                                       dateTime.time().toString("h':'mm A")));
        }
    } else {
        m_statusLabel->setText("");
    }*/
}

void LinTVScheduler::addProgram()
{
    LinTVProgramEditor dialog(m_devicesList);

    // let's get a new program name...
    QString name("LinTV_Record");
    QList<QTreeWidgetItem *> items;
    QStringList itemNames;
    items = m_programsWidget->findItems(name, Qt::MatchStartsWith, 0);
    for (int i = 0; i < items.size(); i++) {
        itemNames << items[i]->text(0);
    }
    
    for (int i = 0; i < items.size(); i++) {
        QString str = QString("LinTV_Record_00%1").arg(i + 1);

        if (!itemNames.contains(str)) {
            name = str;
            break;
        }
    }

    kDebug() << "Setting....";
    dialog.setProgramName(name);

    if (dialog.exec() == QDialog::Accepted) {
        LinTVProgram program = dialog.program();
        helpLabel->hide();
        m_programHandler->addProgram(program);
        addItem(program);

        actionCollection()->action("edit_program")->setEnabled(true);
        actionCollection()->action("remove_program")->setEnabled(true);
    }

    updateStatusBar();
}

void LinTVScheduler::editProgram()
{
    QTreeWidgetItem *item = m_programsWidget->currentItem();

    if (!item) {
        return;
    }

    if (!item->isSelected()) {
        return;
    }

    QString programName = item->text(0); 
    LinTVProgram program = LinTVProgramHandler::findProgram(programName);
    LinTVProgramEditor dialog(program, m_devicesList);

    if (dialog.exec() == QDialog::Accepted) {
        program = dialog.program();
        m_programHandler->updateProgram(programName, program);
        updateItem(item, program);
    }
    updateStatusBar();
}

void LinTVScheduler::removeProgram()
{
    QTreeWidgetItem *item = m_programsWidget->currentItem();
    if (!item) {
        return;
    }
    if (!item->isSelected()) {
        return;
    }

    int rc;
    rc = KMessageBox::questionYesNo(this, i18n("Are you sure you want to remove \"%1\"?", item->text(0)),
                                   i18n("Confirmation"), KStandardGuiItem::yes(), KStandardGuiItem::no(), 
                                    "DontAskAgain");
    
    if (rc == KMessageBox::Yes) {
        LinTVProgram program = m_programHandler->findProgram(item->text(0));

        // If the program is currently recording, tell LinTV to cancel it....
        if (program.status == LinTVProgram::Recording) {
            /*QDBusConnection::sessionBus().send(QDBusMessage::createMethodCall("org.kde.lintv",
                                              "/LinTV",
                                              "",
                                              "cancelRecording"));*/
        }
        m_programHandler->removeProgram(program);
        delete item;
    }

    updateStatusBar();
    refreshPrograms();
}

void LinTVScheduler::refreshPrograms()
{
    // Clear old items
    m_programsWidget->clear();

    LinTVProgramHandler::reloadProgramList();

    // Load the new programs!
    loadPrograms();

    if (LinTVProgramHandler::programList().size() == 0) {
        actionCollection()->action("edit_program")->setEnabled(false);
        actionCollection()->action("remove_program")->setEnabled(false);
    }

    updateStatusBar();
}

void LinTVScheduler::contextMenuEvent(QContextMenuEvent *event)
{
    KMenu m_contextMenu;
    QTreeWidgetItem *item = m_programsWidget->currentItem();
    //QTreeWidgetItem *item = m_programsWidget->itemAt(event->pos());
    if (!item || !item->isSelected()) {
        m_contextMenu.addAction(actionCollection()->action("add_program"));
        m_contextMenu.addAction(actionCollection()->action("refresh_programs"));
        m_contextMenu.addSeparator();
        m_contextMenu.addAction(actionCollection()->action("file_quit"));
        m_contextMenu.exec(event->globalPos());
    } else {
        if (item->isSelected()) {
            m_contextMenu.addAction(actionCollection()->action("edit_program"));
            m_contextMenu.addAction(actionCollection()->action("remove_program"));
            m_contextMenu.addAction(actionCollection()->action("refresh_programs"));
            m_contextMenu.addSeparator();
            m_contextMenu.addAction(actionCollection()->action("file_quit"));

            m_contextMenu.setTitle(item->text(0));
            m_contextMenu.exec(event->globalPos());
        }
    }
}

void LinTVScheduler::loadPrograms()
{
    LinTVProgramList programsList = LinTVProgramHandler::programList();

    for (int i = 0; i < programsList.size(); i++) {
        addItem(programsList[i]);
    }

    if (programsList.size() == 0) {
        helpLabel->show();
        actionCollection()->action("edit_program")->setEnabled(false);
        actionCollection()->action("remove_program")->setEnabled(false);
    } else {
        helpLabel->hide();
    }
}

int LinTVScheduler::detectDevices()
{
    LinTVDeviceInfo deviceInfo;
    m_devicesList = deviceInfo.detectDevices();

    if (m_devicesList.size() == 0) {
	KMessageBox::error(this, i18n("No devices were detected."),
				 i18n("No devices."));
    }
    return m_devicesList.size();
}
#include "lintvscheduler.moc"
