 
#include "lintvprogramwidget.h"
#include "lintvtools.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include <QMouseEvent>

LinTVProgramWidget::LinTVProgramWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    QStringList headerLabels;
    headerLabels << "Program" << "Next Run" << "Duration" << "Status" << "Device";
    setMinimumSize(580, 240);
    setAutoFillBackground(true);
    setColumnCount(4);
    setHeaderLabels(headerLabels);
    //m_programsTableWidget->setSortingEnabled(true);
    //m_programsTableWidget->setAlternatingRowColors(true);
    setRootIsDecorated(false);
    setUniformRowHeights(true);

    setColumnWidth(0, 120);
    setColumnWidth(1, 120);
    setColumnWidth(2, 60);
    setColumnWidth(3, 80);
    setColumnWidth(4, 120);

    QString stylesheetPath = KStandardDirs::locate("data", "lintvscheduler/stylesheets/style.css");
    QString picsDir = KStandardDirs::locate("data", "lintvscheduler/pics/");

    QString stylesheet = LinTVTools::loadStyleSheet(stylesheetPath, picsDir);
    if (stylesheet == "Could not find file!") {
        KMessageBox::error(this, i18n("Could not find the stylesheet.\n"
                                      "Please make sure you have installed LinTV correctly."), 
                                       i18n("Error finding file."));
    } else {
        setStyleSheet(stylesheet);
    }
}

void LinTVProgramWidget::setItemColor(QTreeWidgetItem *item, QColor color)
{
    for (int i = 0; i < 5; i++) {
        item->setBackground(i, QBrush(color, Qt::SolidPattern));
    }
}

void LinTVProgramWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!itemAt(event->pos())) {
        return;
    }

    emit itemDoubleClick();
}

#include "lintvprogramwidget.moc"