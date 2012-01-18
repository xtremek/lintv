/***************************************************************************
   This file is part of LinTV
   lintvprogram.h
   A class which allows for adding, editing, removing, or updating schedules
   from the both the scheduler daemon (atd) and the LinTV configuration.

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
 
#ifndef LINTVPROGRAM_H
#define LINTVPROGRAM_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QStringList>

#define PROGRAM_DAY_SUNDAY      "sun"
#define PROGRAM_DAY_MONDAY      "mon"
#define PROGRAM_DAY_TUESDAY     "tue"
#define PROGRAM_DAY_WEDNESDAY   "wed"
#define PROGRAM_DAY_THURSDAY    "thu"
#define PROGRAM_DAY_FRIDAY      "fri"
#define PROGRAM_DAY_SATURDAY    "sat"

class KConfig;
class QString;

class LinTVProgram 
{
public:
    enum Type { OneTimeOnly = 0x0, EveryDay, EveryWeek };
    enum Status { Queued = 0x0, Recording };


    LinTVProgram() {};

    QString name;
    LinTVProgram::Type type;
    
    QDateTime startDateTime;
    QDateTime endDateTime;
    QStringList daysToRun;

    QString device;
    int input;
    int channel;

    // Keeps track of the current status of the program
    LinTVProgram::Status status;

    // This keeps track of the times that the program has recorded
    int recNumber;
};

typedef QList<LinTVProgram> LinTVProgramList;

class LinTVProgramHandler : public QObject
{
    Q_OBJECT

public:
    LinTVProgramHandler();
    ~LinTVProgramHandler();

    static void init();

    static LinTVProgram createProgram(QString, LinTVProgram::Type, QDateTime,
                           QDateTime, QStringList, QString, int, int);

    void addProgram(QString, LinTVProgram::Type, QDateTime, QDateTime, 
                    QStringList, QString, int, int);
    void addProgram(const LinTVProgram &program);

    void updateProgram(QString, const LinTVProgram &);

    void removeProgram(const LinTVProgram & );

    void startedRecording(const LinTVProgram & );
    void stoppedRecording(const LinTVProgram & );

    static LinTVProgramList programList() { 
        init(); 
        return m_programList; 
    }

    static void reloadProgramList() {
        m_initialized = false;
        init();
    }
    
    static void saveJobs();

    static LinTVProgram findProgram(const QString & );
    static QDateTime nextRunTime(const LinTVProgram & );

    static QString dateTimeFormat() { return m_dateTimeFormat; }
    static QString dateFormat() { return m_dateFormat; }

    static QString toString(LinTVProgram::Status );
    static QString toString(LinTVProgram::Type );
    static LinTVProgram::Status toStatus(QString );
    static LinTVProgram::Type toType(QString );
    static QString createProgramStr(const LinTVProgram &program);

    static QString dayToString(int );
    static Qt::DayOfWeek stringToDay(QString );

private:

    void updateConfigProgram(QString, const LinTVProgram &);
    void removeConfigProgram(QString );
    void writeProgramConfig(const LinTVProgram & );

private:
    KConfig *m_programsConfig;

    static LinTVProgramList m_programList;
    static bool m_initialized;

    static QString m_dateTimeFormat;
    static QString m_dateFormat;
};


#endif