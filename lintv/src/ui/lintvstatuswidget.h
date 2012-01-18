 
#ifndef LINTVSTATUSWIDGET_H
#define LINTVSTATUSWIDGET_H

#include <QWidget>

#include "lintv.h"

class QTimer;

class LinTVStatusWidget : public QWidget
{
    Q_OBJECT
public:

    LinTVStatusWidget(QWidget *parent = 0);

    void start();
    void stop();

    void resetTimer();

    void setChannel(int channel);
    void setInput(const QString & inputName);

    void setStatus(LinTV::Status status);

    LinTV::Status status() { return m_status; }

private slots:
    void slotIntervalTimer();

protected:
    void paintEvent(QPaintEvent *);


private:
    QTimer * m_intervalTimer;

    QString m_inputName;
    int m_channel;
    QTime m_time;

    LinTV::Status m_status;
};

#endif