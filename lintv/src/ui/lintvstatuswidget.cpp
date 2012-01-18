
#include "lintvstatuswidget.h"

#include <kdebug.h>
#include <klocale.h>

#include <QPainter>
#include <QTimer>

LinTVStatusWidget::LinTVStatusWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(QSize(100, 32));
    
    m_time = QTime(0, 0, 0);

    m_intervalTimer = new QTimer(this);
    connect(m_intervalTimer, SIGNAL(timeout()), 
            this, SLOT(slotIntervalTimer()));
    m_intervalTimer->setInterval(1000);
}

void LinTVStatusWidget::start()
{
    if (!m_intervalTimer->isActive()) {
        m_intervalTimer->start();
    }
}

void LinTVStatusWidget::stop()
{
    if (m_intervalTimer->isActive()) {
        m_intervalTimer->stop();
    }
}

void LinTVStatusWidget::resetTimer()
{
    m_time = QTime(0, 0, 0);
    update();
}

void LinTVStatusWidget::setInput(const QString & inputName)
{
    resetTimer();

    m_inputName = inputName;

    update();
}

void LinTVStatusWidget::setChannel(int channel)
{
    m_channel = channel;

    update();
}

void LinTVStatusWidget::setStatus(LinTV::Status status)
{
    m_status = status;

    if (status == LinTV::Playing) {
        start();
    } else if (status == LinTV::Paused) {
        stop();
    } else if (status == LinTV::NotPlaying) {
        stop();
    } else if (status == LinTV::Recording) {
        start();
    } else if (status == LinTV::NoDevices) {
        stop();
    }

    update();
}

void LinTVStatusWidget::slotIntervalTimer()
{
    if (m_status != LinTV::NotPlaying && m_status != LinTV::NoDevices) {
        m_time = m_time.addSecs(1);
    }

    update();
}


void LinTVStatusWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.save();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QBrush(Qt::black));

    // Draw the background
    painter.drawRoundedRect(QRect(0, 0, width(), height()), 4.0, 5.0);

    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Sans Serif", 12, QFont::Bold));

    QString mode;
    if (m_status == LinTV::NotPlaying) {
        mode = "Not Playing";
    } else if (m_status == LinTV::NoDevices) {
        mode = "No devices";
    } else if (m_status == LinTV::Paused) {
        mode = "Paused";
    } else if (m_status == LinTV::Playing) {
        mode = "Playing";
    } else if (m_status == LinTV::Recording) {
        mode = "Recording";
    }

    painter.drawText(12, 16, i18n("%1", mode));

    QString timeStr = QString("%1:%2:%3").arg(m_time.hour(), 2, 10, QChar('0'))
                                      .arg(m_time.minute(), 2, 10, QChar('0'))
                                      .arg(m_time.second(), 2, 10, QChar('0'));

    painter.setFont(QFont("Sans Serif", 8));
    painter.drawText(18, 28, timeStr);

    painter.restore();
}