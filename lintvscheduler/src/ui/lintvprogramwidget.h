 
#ifndef LINTVPROGRAMWIDGET_H
#define LINTVPROGRAMWIDGET_H

#include <QTreeWidget>

class LinTVProgramWidget : public QTreeWidget 
{
    Q_OBJECT
public:
    LinTVProgramWidget(QWidget *parent = 0);

    void setItemColor(QTreeWidgetItem *item, QColor color);

signals:
    void itemDoubleClick();

protected:
    void mouseDoubleClickEvent(QMouseEvent *);

};

#endif