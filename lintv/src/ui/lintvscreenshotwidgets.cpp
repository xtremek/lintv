/***************************************************************************
   This file is part of LinTV
   lintvscreenshotwidgets.cpp
   Contains the ScreenshotsListWidget, ScreenshotLoader, and ScreenshotViewer
   classes. 

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

/* LinTV includes */
#include "lintvscreenshotwidgets.h"
#include "lintvrenamedialog.h"
#include "lintvtools.h"
#include "lintvsettings.h"

/* KDE includes */
#include <kstandardaction.h>
#include <kaction.h>
#include <kurl.h>
#include <kstandarddirs.h>
#include <kio/job.h>
#include <kio/deletejob.h>
#include <kopenwithdialog.h>
#include <krun.h>
#include <kpropertiesdialog.h>
#include <kfileitem.h>
#include <kservice.h>
#include <kmenu.h>
#include <kdebug.h>
#include <kdirwatch.h>
#include <kmacroexpander.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kimageio.h>
#include <kio/netaccess.h>


/* Qt includes */
#include <QDir>
#include <QFileInfo>
#include <QContextMenuEvent>
#include <QPushButton>
#include <QPainter>
#include <QPalette>


ScreenshotLoader::ScreenshotLoader(QObject *parent)
    : QThread(parent)
{
}

void ScreenshotLoader::run()
{
    QStringList screenshotsList = LinTVSettings::screenshotNames();

    //kDebug() << screenshotsList;
    for (int i = 0; i < screenshotsList.size(); i++)
    {
        msleep(50);
        QFileInfo fileInfo(screenshotsList[i]);
        // If the file doesn't exist, just remove it from the list
        if (!fileInfo.exists()) {
            screenshotsList.removeAt(i);
            continue;
        }

        emit foundScreenshot(fileInfo.fileName(), screenshotsList[i]);
    }

    // Update this just in case a file wasn't found and was removed....
    LinTVSettings::setScreenshotNames(screenshotsList);
}

ScreenshotListWidget::ScreenshotListWidget(QWidget *parent)
  : KListWidget(parent)
{
    setViewMode(QListView::IconMode);
    setMovement(QListView::Static);
    setSpacing(4);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setIconSize(QSize(86, 86));

    QString themesDirectory = KStandardDirs::locate("data", "lintv/themes/style.css");
    QString picsDirectory = KStandardDirs::locate("data", "lintv/pics/");
    QFile file(themesDirectory);

    QString styleSheet;
    QString expandedStyleSheet;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        styleSheet = QString::fromUtf8(file.readAll());
        QHash<QString, QString> macros;
        macros["picsDir"] = picsDirectory;
        expandedStyleSheet = KMacroExpander::expandMacros(styleSheet, macros, QLatin1Char('$'));
        //setStyleSheet(expandedStyleSheet);
    }

    setMouseTracking(true);

    connect(this, SIGNAL(itemEntered(QListWidgetItem *)),
            this, SLOT(itemMouseEnter(QListWidgetItem *)));
    //kDebug() << expandedStyleSheet;

    //setFont(QFont("Sans Serif", 9, QFont::Normal));

    m_screenshotsList = new KFileItemList();
    m_screenshotWatcher = new KDirWatch();
    connect(m_screenshotWatcher, SIGNAL(deleted(const QString &)),
            this, SLOT(deletedScreenshot(const QString & )));
    connect(m_screenshotWatcher, SIGNAL(dirty(const QString &)),
            this, SLOT(dirtyScreenshot(const QString & )));

    // Load all the actions
    viewScreenshotAction = new KAction(KIcon("image-x-generic"),
                                        i18n("View Screenshot"), this);
    connect(viewScreenshotAction, SIGNAL(triggered()),
            this, SLOT(viewScreenshot()));
    saveAsAction = new KAction(KIcon("document-save-as"),
                             i18n("Save As..."), this);
    connect(saveAsAction, SIGNAL(triggered()),
            this, SLOT(saveScreenshotAs()));

    /*takeScreenshotAction = new KAction(KIcon("camera-photo"),
                                       i18n("Take Snapshot"), this);*/
    
    renameAction = new KAction(KIcon("edit-rename"),
                               i18n("Rename"), this);
    connect(renameAction, SIGNAL(triggered()),
            this, SLOT(renameScreenshot()));

    deleteAction = new KAction(KIcon("edit-delete"),
                               i18n("Delete"), this);
    deleteAction->setShortcut(Qt::Key_Delete);
    connect(deleteAction, SIGNAL(triggered()),
            this, SLOT(deleteScreenshot()));

    deleteAllAction = new KAction(KIcon("edit-clear"),
                                  i18n("Delete All"), this);
    deleteAllAction->setShortcut(QKeySequence(Qt::ShiftModifier | Qt::Key_Delete));
    connect(deleteAllAction, SIGNAL(triggered()),
            this, SLOT(deleteAllScreenshots()));

    propertiesAction = new KAction(KIcon("document-properties"),
                                   i18n("Properties"), this);
    connect(propertiesAction, SIGNAL(triggered()),
            this, SLOT(showProperties()));

    m_contextMenu = new KMenu(this);
    m_contextMenu->addAction(viewScreenshotAction);
    //menu->addAction(takeScreenshotAction);

    // Let's do the "Open With" menu here...
    m_openWithMenu = new KMenu(i18n("Open With..."), this);
    connect(m_openWithMenu, SIGNAL(triggered(QAction *)),
            this, SLOT(openWith(QAction *)));

    m_contextMenu->addMenu(m_openWithMenu);
    m_contextMenu->addAction(saveAsAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(renameAction);
    m_contextMenu->addAction(deleteAction);
    m_contextMenu->addAction(deleteAllAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(propertiesAction);

    loader = new ScreenshotLoader;
    connect(loader, SIGNAL(foundScreenshot(QString, QString )),
            this, SLOT(foundScreenshot(QString, QString )));

    loadScreenshots();
}

ScreenshotListWidget::~ScreenshotListWidget()
{
    delete m_screenshotsList;
    delete m_screenshotWatcher;
    delete loader;
}

void ScreenshotListWidget::viewScreenshot()
{
    if (currentItem()->isSelected()) {
        QString filename = m_screenshotsList->at(currentRow()).url().path();
        //kDebug() << filename;
        emit displayScreenshot(QPixmap(filename));
    }
}

void ScreenshotListWidget::saveScreenshotAs()
{
    const KUrl& url = m_screenshotsList->at(currentRow()).url();
    KFileDialog dialog(url, QString(), this);

    dialog.setOperationMode(KFileDialog::Saving);
    dialog.setSelection(url.fileName());
    dialog.setMimeFilter(KImageIO::mimeTypes(KImageIO::Writing),
                         KMimeType::findByUrl(url)->name());

    if (!dialog.exec()) {
        return;
    }

    const QString mimeType = dialog.currentMimeFilter();
    if (mimeType.isEmpty()) {
        KMessageBox::sorry(this, i18nc("@info", "No image format selected."));
        return;
    }

    KUrl saveAsUrl = dialog.selectedUrl();
    if (KIO::NetAccess::exists(saveAsUrl, KIO::NetAccess::DestinationSide, this)) {
        int answer = KMessageBox::warningContinueCancel(
            this,
            i18nc("@info",
                "A file named <filename>%1</filename> already exists.\n"
                "Are you sure you want to overwrite it?",
                saveAsUrl.fileName()),
            QString(),
            KStandardGuiItem::overwrite());
        if (answer == KMessageBox::Cancel) {
            return;
        }
    }

    QFile file(url.path());
    file.copy(saveAsUrl.path());
}

void ScreenshotListWidget::addScreenshot(QPixmap image)
{
    QString screenshotName = nextScreenshotName();
    KUrl fileUrl(screenshotName);

    QString fileName = fileUrl.fileName();

    // Add the item to the list widget
    QListWidgetItem *item = new QListWidgetItem(image.scaled(QSize(86, 86), 
                                                          Qt::KeepAspectRatio, 
                                                          Qt::FastTransformation),
                                                fileName);
    checkTextSize(item);
    insertItem(m_currentScreenshot, item);

    // Save the screenshot...    
    image.save(screenshotName);

    // Now add the image to the index
    m_screenshotsList->insert(m_currentScreenshot, KFileItem(KFileItem::Unknown, 
                                                             KFileItem::Unknown,
                                                             fileUrl, true));

    // Finally add it to the KDirWatcher
    m_screenshotWatcher->addFile(screenshotName);

    updateScreenshotList();
}

void ScreenshotListWidget::renameScreenshot()
{
    if (!currentItem()->isSelected()) {
        return;
    }

    int curRow = currentRow();
    const KFileItem& item = m_screenshotsList->at(curRow);
    LinTVRenameDialog dialog(this, item);
    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    const QString newName = dialog.newName();
    if (newName.isEmpty()) {
        return;
    }

    const KUrl& oldUrl = item.url();
    QString name = newName;
    if (oldUrl.fileName() != name) {
        KUrl newUrl = oldUrl;
        newUrl.setFileName(name);

        KFileItem curItem = m_screenshotsList->at(curRow);
        curItem.setUrl(newUrl);
        //kDebug() << curItem.url().path();
        m_screenshotsList->removeAt(curRow);
        m_screenshotsList->insert(curRow, curItem);
        currentItem()->setText(newUrl.fileName());

        checkTextSize(currentItem());

        QFile file(oldUrl.path());
        file.rename(newUrl.path());

        //kDebug() << "Old:" << oldUrl.path();
        //kDebug() << "New:" << newUrl.path();
        updateScreenshotList();
    }

}

void ScreenshotListWidget::deleteScreenshot()
{
    if (!currentItem()->isSelected()) {
        return;
    }

    KIO::del(m_screenshotsList->at(currentRow()).url());

    setToolTip("");
}

void ScreenshotListWidget::deleteAllScreenshots()
{
    for (int i = 0; i < m_screenshotsList->size(); i++) {
        KIO::del(m_screenshotsList->at(i).url());   
    }
}

void ScreenshotListWidget::deletedScreenshot(const QString & path)
{
    int screenshotIndex = findScreenshotIndex(path);

    // Remove the screenshot
    takeItem(screenshotIndex);

    m_screenshotsList->removeAt(screenshotIndex);
    m_screenshotWatcher->removeFile(path);

    if (count() == 0) {
        emit deletedAllScreenshots();
    } else {
        emit deletedScreenshot(currentItem()->icon().pixmap(LinTVSettings::screenshotWidth(), 
                                                        LinTVSettings::screenshotHeight()));
    }

    updateScreenshotList();
}

void ScreenshotListWidget::dirtyScreenshot(const QString & path)
{
    int index = findScreenshotIndex(path);
    QListWidgetItem *dirtyItem = item(index);
    dirtyItem->setIcon(QPixmap(path));
    dirtyItem->setText(m_screenshotsList->at(index).url().path());
}

int ScreenshotListWidget::findScreenshotIndex(const QString & path)
{
    for (int i = 0; i < m_screenshotsList->size(); i++) {
        if (path == m_screenshotsList->at(i).url().path()) {
            return i;
        }
    }

    return -1;
}

void ScreenshotListWidget::checkTextSize(QListWidgetItem * item)
{
    QString text = item->text();
    QFontMetrics fm(font());
    int textWidth;

    if ((textWidth = fm.width(text)) > 112) {
        //kDebug() << textWidth;
        for (int i = textWidth; i >= 96; i--) {
            text.remove(text.length() - 1, 1);
            i = fm.width(text);
        }
        text.append("...");
    }
    item->setText(text);
}

QString ScreenshotListWidget::nextScreenshotName()
{
    QString screenshotName;
    QString screenshotStr;

    for (int i = 0; i < 6000; i++) {
        screenshotName = screenshotName.sprintf("LinTVSnap%03d.%s", i, 
                                                qPrintable(LinTVSettings::screenshotFormat().toLower()));
        screenshotStr =  LinTVSettings::screenshotDirectory() + "/" + screenshotName;
        //kDebug() << screenshotStr;
        if (!QFile::exists(screenshotStr)) {
            m_currentScreenshot = i;
            m_screenshotName = screenshotStr;
            return screenshotStr;
        }
    }

    return QString();
}

void ScreenshotListWidget::updateScreenshotList()
{
    QStringList list;

    KUrl::List screenshotsList = m_screenshotsList->urlList();
    for (int i = 0; i < screenshotsList.size(); i++) {
        list << screenshotsList[i].path();
        //kDebug() << screenshotsList[i].path();
    }

    LinTVSettings::setScreenshotNames(list);
}

void ScreenshotListWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    if (currentRow() > m_screenshotsList->size() || currentRow() < 0) {
        return;
    }
    viewScreenshot();
}

void ScreenshotListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (currentRow() > m_screenshotsList->size() || currentRow() < 0) {
        return;
    }
  
    loadOpenWithMenu();

    m_contextMenu->exec(event->globalPos());
}

// I took the code from the Gwenview openWith function in
// fileopscontextmanageritem.cpp....
void ScreenshotListWidget::openWith(QAction *action)
{
    if (!currentItem()->isSelected()) {
        return;
    }


    Q_ASSERT(action);
    KService::Ptr service;
    KFileItem item = m_screenshotsList->at(currentRow());

    QString name = action->data().toString();
    if (name.isEmpty()) {
        KOpenWithDialog dlg(item.url(), this);
        if (!dlg.exec()) {
            return;
        }
        service = dlg.service();

        if (!service) {
            Q_ASSERT(!dlg.text().isEmpty());
            KRun::run(dlg.text(), item.url(), this);
            return;
        }
    } else {
        service = LinTVTools::serviceForName(item)[name];
    }

    Q_ASSERT(service);
    KRun::run(*service, item.url(), this);
}

void ScreenshotListWidget::showProperties()
{
    if (!currentItem()->isSelected()) {
        return;
    }

    KPropertiesDialog::showDialog(m_screenshotsList->at(currentRow()).url(), this);
}

void ScreenshotListWidget::foundScreenshot(QString name, QString filename)
{
    QPixmap screenshot = QPixmap(filename).scaled(QSize(86, 86), Qt::KeepAspectRatio, 
                                                                Qt::FastTransformation);


    QListWidgetItem *item = new QListWidgetItem(screenshot, name);
    checkTextSize(item);
    // Add the item to the widget
    addItem(item);

    // Add a new file item to the list...
    m_screenshotsList->append(KFileItem(KFileItem::Unknown, KFileItem::Unknown,
                                      KUrl(filename), true));

    // We want to watch this file...
    m_screenshotWatcher->addFile(filename);

    //kDebug() << name;
    //kDebug() << filename;
}

void ScreenshotListWidget::loadScreenshots()
{
    // Clear all existing items
    m_screenshotsList->clear();
    clear();

    QDir dir(LinTVSettings::screenshotDirectory());
    if (!dir.exists()) {
        dir.mkdir(LinTVSettings::screenshotDirectory());
    }

    QStringList screenshotsList = LinTVSettings::screenshotNames();

    loader->start();
}

void ScreenshotListWidget::loadOpenWithMenu()
{
    if (currentRow() > m_screenshotsList->size() || currentRow() < 0) {
        return;
    }
    qDeleteAll(m_openWithMenu->actions());

    QMap<QString, KService::Ptr> serviceForName;

    serviceForName = LinTVTools::serviceForName(m_screenshotsList->at(currentRow()));

    foreach (const KService::Ptr &service, serviceForName) {
        QString text = service->name().replace("&", "&&");
        KAction *action = new KAction(KIcon(service->icon()),
                                      text, this);
        action->setData(service->name());
        m_openWithMenu->addAction(action);
    }

    m_openWithMenu->addSeparator();
    m_openWithMenu->addAction(i18n("Other Application..."));
}

KMenu* ScreenshotListWidget::contextMenu()
{
    loadOpenWithMenu(); 

    // We don't want to show the View Screenshot action so we just create a new menu here...
    KMenu* menu = new KMenu(i18n("Screenshot Operations"));

    menu->setIcon(KIcon("image-x-generic"));

    menu->addMenu(m_openWithMenu);
    menu->addAction(saveAsAction);
    menu->addSeparator();
    menu->addAction(renameAction);
    menu->addAction(deleteAction);
    menu->addAction(deleteAllAction);
    menu->addSeparator();
    menu->addAction(propertiesAction);
    return menu;
}

void ScreenshotListWidget::itemMouseEnter(QListWidgetItem * item)
{
    QString tooltipStr;
    int itemRow = row(item);

    tooltipStr = "Name: " + m_screenshotsList->at(itemRow).url().path();
    setToolTip(tooltipStr);
}

ScreenshotViewer::ScreenshotViewer(QWidget *parent)
    : QWidget(parent)
{
    returnToTVPushButton = new QPushButton(KIcon("go-previous"),
                                               i18n("Return to TV"), this);
    returnToTVPushButton->move(20, (40 / 2) - (returnToTVPushButton->height() / 2));
    connect(returnToTVPushButton, SIGNAL(clicked()),
            this, SIGNAL(returnToTV()));
}

/*void ScreenshotViewer::returnToTVPushButtonClicked()
{
}*/

void ScreenshotViewer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.save();

    //kDebug() << x() << y() << width() << height();
    if (!m_pixmap.isNull()) {
        painter.drawPixmap(0, 0, m_pixmap.scaled(width(), height()));
    }

    // Draw the top bar...
    /*painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::black));
    painter.setOpacity(0.2);
    painter.drawRect(QRect(0, 0, width(), 40));*/

    painter.restore();
}


#include "lintvscreenshotwidgets.moc"