/***************************************************************************
   This file is part of LinTV
   lintvrenamedialog.h
   A file renaming dialog for LinTV based on Dolphin's rename dialog. Code
   taken from renamedialog.h in Dolphin source code.

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

#ifndef LINTVRENAMEDIALOG_H
#define LINTVRENAMEDIALOG_H

#include <kdialog.h>
#include <kurl.h>

class KFileItem;
class KLineEdit;

class LinTVRenameDialog : public KDialog
{
    Q_OBJECT
public:
    LinTVRenameDialog(QWidget *parent, const KFileItem item);
    ~LinTVRenameDialog();

    const QString & newName() const
    { 
        return m_newName;
    }
    
protected slots:
    void slotButtonClicked(int button);

private:
    KLineEdit* m_lineEdit;
    QString m_newName;
};

#endif