/***************************************************************************
   This file is part of LinTV
   lintvview.cpp
   The main widget for LinTV.

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

/* Program includes */
#include "lintvview.h"
#include "lintvtools.h"
#include "lintvscreenshotwidgets.h"
#include "lintvsettings.h"
#include "lintvconfigdialog.h"
#include "lintvstatuswidget.h"

/* KDE includes */
#include <klocale.h>
#include <kapplication.h>
#include <kconfigdialog.h>
#include <kstatusbar.h>
#include <kaction.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kstandardaction.h>
#include <khelpmenu.h>
#include <kmenu.h>
#include <kxmlguibuilder.h>
#include <kxmlguifactory.h>
#include <kwindowsystem.h>
#include <netwm.h>
#include <kmessagebox.h>

/* Qt includes */
#include <QTimer>
#include <QLabel>
#include <QActionGroup>
#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QStackedLayout>

bool LinTVView::m_initConfigDialog = false;

LinTVView::LinTVView(QStringList devices)
    : KXmlGuiWindow()
{
    m_devices = devices;
    m_recordMode = false;

    widget = new QWidget(this);

    // Setup the user interface
    view.setupUi(widget);
    setCentralWidget(widget);

    m_fullscreen = false;
    m_dragging = false;

    videoOutput = new QLabel(this);
    videoOutput->setParent(view.backgroundWidget);
    videoOutput->setMouseTracking(true);

    // Setup the buttons and icons and stuff
    setupLinTVUi();

    m_cursorHideTimer = new QTimer(this);
    m_cursorHideTimer->start(150);

    connectSignalsSlots();

    resize(LinTVSettings::width(), LinTVSettings::height());
    checkSize();
}

LinTVView::~LinTVView()
{
    delete m_screenshotListWidget;

    saveSettings();
}

void LinTVView::setDeviceIndex(int index)
{
    m_devicesActionGroup->actions()[index]->setChecked(true);
}

void LinTVView::connectSignalsSlots()
{
    connect(view.recordToolButton, SIGNAL(toggled(bool )),
            this, SLOT(slotRecordButtonToggled(bool )));
    connect(view.upChannelPushButton, SIGNAL(clicked()),
            this, SLOT(slotChannelUp()));
    connect(view.downChannelPushButton, SIGNAL(clicked()),
            this, SLOT(slotChannelDown()));
    connect(view.takeScreenshotButton, SIGNAL(clicked()),
            this, SLOT(slotTakeScreenshot()));

    connect(m_cursorHideTimer, SIGNAL(timeout()),
            this, SLOT(slotCursorHideTimer()));

    connect(m_screenshotViewer, SIGNAL(returnToTV()),
            this, SLOT(slotReturnToTV()));
    connect(m_screenshotListWidget, SIGNAL(displayScreenshot(QPixmap)),
            this, SLOT(slotDisplayScreenshot(QPixmap)));
    connect(m_screenshotListWidget, SIGNAL(deletedScreenshot(QPixmap)),
            this, SLOT(slotRemovedScreenshot(QPixmap)));
    connect(m_screenshotListWidget, SIGNAL(deletedAllScreenshots()),
            this, SLOT(slotReturnToTV()));

    connect(view.tvModePushButton, SIGNAL(clicked()),
            actionCollection()->action("tv_mode"), SLOT(trigger()));
    connect(view.alwaysOnTopPushButton, SIGNAL(toggled(bool )),
            this, SLOT(slotSetAlwaysOnTop(bool)));
    connect(view.volumeSlider, SIGNAL(valueChanged(int )),
            this, SLOT(slotSetVolume(int )));
    connect(view.mutePushButton, SIGNAL(toggled(bool )),
            this, SLOT(slotToggleMute(bool )));
    connect(view.pauseToolButton, SIGNAL(toggled(bool )),
            this, SLOT(slotTogglePause(bool )));
    connect(view.inputSelector, SIGNAL(activated(int )),
            this, SLOT(slotSetInput(int )));
}


void LinTVView::saveSettings()
{
    if (!m_fullscreen) {
        LinTVSettings::setWidth(width());
        LinTVSettings::setHeight(height());
    }

    LinTVSettings::setVolume(view.volumeSlider->value());
    LinTVSettings::setMuted(view.mutePushButton->isChecked());
    LinTVSettings::setLastInput(view.inputSelector->currentIndex());

    LinTVSettings::self()->writeConfig();
}

void LinTVView::keyPressEvent(QKeyEvent *event)
{
    if (m_fullscreen && event->key() == Qt::Key_Escape) {
        actionCollection()->action("tv_mode")->trigger();
    }
}

void LinTVView::checkSize()
{
    int w, h;
    float aspectRatio = 4.0/3.0;


    QWidget *widget = view.backgroundWidget;

    w = widget->width();
    h = widget->width() / aspectRatio;

    if (h > widget->height()) {
        h = widget->height();
        w = widget->height() * aspectRatio;
    }

    videoOutput->resize(w, h);    
    videoOutput->move((widget->width() - w) / 2,(widget->height() - h) / 2);
}

void LinTVView::resizeEvent(QResizeEvent *)
{
    checkSize();

    // Also resize the screenshot viewer if it's being shown
    if (m_screenshotViewer->isVisible()) {
        m_screenshotViewer->move(videoOutput->pos());
        m_screenshotViewer->resize(videoOutput->size());
    }
}

void LinTVView::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget *widget = qApp->widgetAt(event->globalPos());
    if (widget == videoOutput || widget == view.backgroundWidget) {
        m_contextMenu->exec(event->globalPos());
    } else if (widget == m_screenshotViewer) {
        m_screenshotListWidget->reloadMenus();
        m_screenshotContextMenu->exec(event->globalPos());
    }
}

void LinTVView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragOffset = event->pos();
        m_dragging = true;
    }
}

void LinTVView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_fullscreen && m_dragging) {
        move(event->globalPos().x() - m_dragOffset.x(),
             event->globalPos().y() - m_dragOffset.y());
    }

    m_mouseIsMoving = true;
}

void LinTVView::mouseReleaseEvent(QMouseEvent *)
{
    m_dragging = false;
}

void LinTVView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget *widget = qApp->widgetAt(event->globalPos());
    if (widget == videoOutput || widget == view.backgroundWidget 
        || widget == m_screenshotViewer) {
        actionCollection()->action("tv_mode")->trigger();
    }
    setCursor(QCursor(Qt::ArrowCursor));
    m_cursorHidden = false;
}

void LinTVView::closeEvent(QCloseEvent *event)
{
    event->ignore();

    exitEvent();
}

void LinTVView::setupLinTVUi()
{
    videoOutput->setParent(view.backgroundWidget);

    view.tvModePushButton->setText("");
    view.tvModePushButton->setIcon(KIcon("video-television"));

    view.alwaysOnTopPushButton->setText("");
    view.alwaysOnTopPushButton->setIcon(KIcon("preferences-system-windows"));
    view.alwaysOnTopPushButton->setCheckable(true);

    view.configurePushButton->setText("");
    view.configurePushButton->setIcon(KIcon("configure"));

    view.upChannelPushButton->setText("");
    view.upChannelPushButton->setIcon(KIcon("go-up"));

    view.downChannelPushButton->setText("");
    view.downChannelPushButton->setIcon(KIcon("go-down"));

    view.takeScreenshotButton->setText("");
    view.takeScreenshotButton->setIcon(KIcon("camera-photo"));

    view.pauseToolButton->setText("");
    view.pauseToolButton->setIcon(KIcon("media-playback-pause"));
    view.pauseToolButton->setCheckable(true);

    view.recordToolButton->setText("");
    view.recordToolButton->setIcon(KIcon("record"));
    view.recordToolButton->setCheckable(true);

    view.mutePushButton->setText("");
    view.mutePushButton->setIcon(KIcon("audio-volume-high"));
    view.mutePushButton->setCheckable(true);


    m_screenshotListWidget = new ScreenshotListWidget(this);
    delete view.screenshotsListWidget;
    view.verticalLayout->insertWidget(1, m_screenshotListWidget);

    // Now let's load the actions for the context menus
    KAction *action = new KAction(KIcon("video-television"),
                                  i18n("Toggle TV Mode"),
                                  this);
    action->setShortcut(KShortcut(Qt::CTRL + Qt::Key_F),
                              KAction::DefaultShortcut);
    action->setCheckable(true);

    actionCollection()->addAction("tv_mode", action);
    connect(action, SIGNAL(triggered()), this, SLOT(slotToggleTVMode()));


    action = new KAction(KIcon("go-previous"),
                               i18n("Return to TV"), this);
    actionCollection()->addAction("return_to_tv", action);
    action->setShortcut(KShortcut(Qt::Key_Escape),
                              KAction::DefaultShortcut);

    connect(action, SIGNAL(triggered()),
            this, SLOT(slotReturnToTV()));

    action = new KAction(KIcon("video-color"),
                         i18n("Adjust Video Colors"),
                         this);
    action->setShortcut(KShortcut(Qt::CTRL + Qt::Key_C),
                              KAction::DefaultShortcut);
    actionCollection()->addAction("adjust_colors", action);
    connect(action, SIGNAL(triggered()), this, SIGNAL(showColorDialog()));

    /* The "Select Device" menu */
    action = new KAction(KIcon("audio-card"),
                         i18n("View Device Info"), 
                         this);
    action->setShortcut(KShortcut(Qt::CTRL + Qt::Key_I),
                              KAction::DefaultShortcut);
    actionCollection()->addAction("device_info", action);
    connect(action, SIGNAL(triggered()), this, SLOT(slotShowDevicesInfo()));

    m_devicesActionGroup = new QActionGroup(this);

    for (int i = 0; i < m_devices.size(); i++) {
        action = new KAction(m_devices[i], m_devicesActionGroup);
        action->setCheckable(TRUE);
    }

    if (m_devices.size() == 0) {
        action = new KAction(i18n("No devices detected..."), m_devicesActionGroup);
    }

    // Load the windows sizes menu....
    action = new KAction(i18n("Default Size"), this);
    action->setShortcut(KShortcut(Qt::CTRL + Qt::Key_D),
                              KAction::DefaultShortcut);
    actionCollection()->addAction("default_size", action);
    connect(action, SIGNAL(triggered()),
            this, SLOT(slotSetDefaultSize()));

    // Add some window size presets...
    QActionGroup *m_windowSizeActionGroup = new QActionGroup(this);
    action = new KAction("640x410", m_windowSizeActionGroup);
    action = new KAction("860x568", m_windowSizeActionGroup);
    action = new KAction("1024x700", m_windowSizeActionGroup);
    action = new KAction("1280x900", m_windowSizeActionGroup);

    connect(m_windowSizeActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(slotSetWindowSize(QAction *)));

    // Add the default preferences, help menu, and quit actions
    new KHelpMenu(this, KGlobal::mainComponent().aboutData(), true, 
                        actionCollection());

    KStandardAction::preferences(this, SLOT(slotShowConfigureDialog()), 
                                                actionCollection());
    KStandardAction::quit(this, SLOT(quit()), actionCollection());


    // Now let's load the Ui components 
    setXMLFile(componentData().componentName() + "ui.rc");

    m_guiBuilder = new KXMLGUIBuilder(this);
    m_guiFactory = new KXMLGUIFactory(m_guiBuilder, this);
    m_guiFactory->addClient(this);

    m_contextMenu = static_cast<KMenu *>(m_guiFactory->container("lintv_context",
                                         this));

    m_devicesMenu = static_cast<KMenu *>(m_guiFactory->container("select_device", this));
    connect(m_devicesActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(slotSelectDevice(QAction *)));
    m_devicesMenu->addActions(m_devicesActionGroup->actions());
    m_videoSizeMenu = static_cast<KMenu *>(m_guiFactory->container("video", this));

    m_videoSizeMenu->addActions(m_windowSizeActionGroup->actions());
    connect(view.configurePushButton, SIGNAL(clicked()), 
            this, SLOT(slotShowConfigureDialog()));

    m_screenshotContextMenu = static_cast<KMenu* >(m_guiFactory->container("screenshot_context",
                                                   this));
    m_screenshotContextMenu->insertMenu(actionCollection()->action("options_configure"),
                                        m_screenshotListWidget->contextMenu());
    m_screenshotContextMenu->insertSeparator(actionCollection()->action("options_configure"));


    // Initialize the screenshot viewer
    m_screenshotViewer = new ScreenshotViewer(this);
    m_screenshotViewer->setParent(view.backgroundWidget);
    m_screenshotViewer->hide();

    m_statusWidget = new LinTVStatusWidget(this);
    view.bottomLayout->insertWidget(3, m_statusWidget);
}

void LinTVView::checkInput()
{
    // TODO: Use lintvdevicectl to find out whether the input
    // has a tuner or not.
    if (view.inputSelector->currentText().contains("Television") ||
        view.inputSelector->currentText().contains("Tuner")) {
        view.upChannelPushButton->setEnabled(true);
        view.downChannelPushButton->setEnabled(true);
        view.channelLabel->setEnabled(true);
    } else {
        view.upChannelPushButton->setEnabled(false);
        view.downChannelPushButton->setEnabled(false);
        view.channelLabel->setEnabled(false);
    }
}

void LinTVView::setInput(int value)
{
    view.inputSelector->setCurrentIndex(value);
    checkInput();

    // Show an OSD with the current input name
}

void LinTVView::setRecordMode(bool set)
{
    m_recordMode = set;

    if (set) {
        view.configurePushButton->setEnabled(false);
        view.inputSelector->setEnabled(false);

        actionCollection()->action("adjust_colors")->setEnabled(false);
        actionCollection()->action("options_configure")->setEnabled(false);
    } else {
        view.configurePushButton->setEnabled(true);
        view.inputSelector->setEnabled(true);

        actionCollection()->action("adjust_colors")->setEnabled(true);
        actionCollection()->action("options_configure")->setEnabled(true);
    }
}

void LinTVView::setInputs(QStringList inputs)
{
    view.inputSelector->clear();
    view.inputSelector->addItems(inputs);
    view.inputSelector->setCurrentIndex(0);
}

void LinTVView::setChannel(int channel)
{
    view.channelLabel->setText(i18n("Ch %1", channel));
}

void LinTVView::setVolume(int volume)
{
    slotSetVolume(volume);
}

void LinTVView::setMute(bool mute)
{
    view.mutePushButton->setChecked(mute);
    slotToggleMute(mute);
}

void LinTVView::slotChannelUp()
{
    emit channelUp();
}

void LinTVView::slotChannelDown()
{
    emit channelDown();
}

void LinTVView::slotSetInput(int input)
{
    emit inputChanged(input);
}

void LinTVView::slotToggleTVMode()
{
    if (!m_fullscreen) {
        setWindowState(windowState() | Qt::WindowFullScreen);
        view.mainSidebar->hide();
        view.bottomLayoutWidget->hide();
        view.mainLayout->setContentsMargins(0, 0, 0, 0);
        view.mainLayout->setHorizontalSpacing(0);

        m_fullscreen = true;
    } else {
        setWindowState(Qt::WindowNoState);
        view.mainSidebar->show();
        view.bottomLayoutWidget->show();
        view.mainLayout->setContentsMargins(4, 4, 4, 4);
        view.mainLayout->setHorizontalSpacing(-1);
        m_fullscreen = false;
    }
}


void LinTVView::slotSetAlwaysOnTop(bool on)
{
    unsigned long state = KWindowInfo(KWindowSystem::windowInfo(winId(), 
                                                  NET::WMState)).state();
    if (on) {
        KWindowSystem::setState(winId(), state | NET::KeepAbove);
    } else {
        KWindowSystem::clearState(winId(), NET::KeepAbove);
    }
}

void LinTVView::slotSelectDevice(QAction *action)
{
    emit deviceNameSelected(action->text());
}
  
  
void LinTVView::slotSetDefaultSize()
{
    resize(LINTV_DEFAULT_SIZE);
}

// TODO: Get a better calculation based on video width and height....
void LinTVView::slotSetWindowSize(QAction *action)
{
    // If we're in fullscreen mode, we won't resize the window.
    if (m_fullscreen) {
        return;
    }

    QStringList widthHeight = action->text().split("x");
    int width, height;

    width = widthHeight[0].toInt(NULL, 10);
    height = widthHeight[1].toInt(NULL, 10);

    resize(width, height);
}

void LinTVView::slotShowConfigureDialog()
{
    if (!m_initConfigDialog) {
        m_configDialog = new LinTVConfigDialog(this);

        connect(m_configDialog, SIGNAL(settingsChanged()), 
              this, SIGNAL(settingsChanged()));
        m_initConfigDialog = true;
    }
    m_configDialog->exec();
}

void LinTVView::slotSetVolume(int volume)
{
    view.volumeSlider->setValue(volume);
    view.volumeLabel->setText(i18n("%1 %", volume));
    view.volumeSlider->setToolTip(i18n("Volume %1%", volume));

    emit volumeChanged(volume);
}

void LinTVView::slotToggleMute(bool toggle)
{
    if (toggle) {
        view.mutePushButton->setIcon(KIcon("audio-volume-muted"));
    } else {
        view.mutePushButton->setIcon(KIcon("audio-volume-high"));
    }

    emit muteToggled(toggle);
}

void LinTVView::slotTogglePause(bool pause)
{
    emit pauseToggled(pause);

    if (pause) {
        view.pauseToolButton->setToolTip(i18n("Click to un-pause the video..."));
    } else {
        view.pauseToolButton->setToolTip(i18n("Click to pause the video..."));
    }

    // Show an OSD or something with a pause icon
}


void LinTVView::slotTakeScreenshot()
{
    QPixmap screenshot;
    QSize screenshotSize(LinTVSettings::screenshotWidth(),
                         LinTVSettings::screenshotHeight());

    screenshot = QPixmap::grabWindow(videoOutput->winId());

    // Get the screenshot
    if (screenshotSize.width() != 0 && screenshotSize.height() != 0) {
        screenshot = screenshot.scaled(screenshotSize, Qt::IgnoreAspectRatio, 
                                                    Qt::SmoothTransformation);
    }

    
    m_screenshotListWidget->addScreenshot(screenshot);
}

void LinTVView::slotDisplayScreenshot(QPixmap image)
{
    m_screenshotViewer->setPixmap(image);
    m_screenshotViewer->resize(videoOutput->size());
    m_screenshotViewer->show();
    m_screenshotViewer->raise();
}

void LinTVView::slotRemovedScreenshot(QPixmap newImage)
{
    // If the screenshot viewer is not showing a screenshot,
    // we just skip it...
    if (m_screenshotViewer->isVisible()) {
        m_screenshotViewer->setPixmap(newImage);
    }
}

void LinTVView::slotReturnToTV()
{
    m_screenshotViewer->hide();
    m_screenshotViewer->lower();
}

void LinTVView::setRecordButton(bool on)
{
    view.recordToolButton->setChecked(on);

}

void LinTVView::slotRecordButtonToggled(bool on)
{
    if (on) {
        view.recordToolButton->setToolTip(i18n("Click to stop recording..."));
    } else {
        view.recordToolButton->setToolTip(i18n("Click to start recording..."));
    }

    emit startRecordingToggled(on);
}

void LinTVView::slotCursorHideTimer()
{
    if (!m_fullscreen) {
        return;
    }

    static int time = 0;

    if (m_lastPos != QCursor::pos()) {
        m_cursorHidden = false;
        time = 0;
    } else {
        time++;
        if (time == 11) {
            m_cursorHidden = true;
            time = 0;
        }
    }

    if (m_cursorHidden) {
        setCursor(QCursor(Qt::BlankCursor));
    } else {
        setCursor(QCursor(Qt::ArrowCursor));
    }

    m_lastPos = QCursor::pos();
}

void LinTVView::slotShowDevicesInfo()
{
    emit showDeviceInfoDialog();
}

void LinTVView::quit()
{
    exitEvent();
}
#include "lintvview.moc"
