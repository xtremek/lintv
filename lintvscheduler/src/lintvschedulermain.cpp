/***************************************************************************
   This file is part of LinTV Scheduler
   lintvschedulermain.cpp
   The file containing the main() function for LinTV Scheduler

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
#include "lintvglobal.h"

#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>
#include <kdebug.h>

#include <X11/Xlib.h>

static const char description[] = 
    I18N_NOOP("A KDE 4 application for sheduling TV recordings for LinTV.");

static const char version[] = "0.1.2 Alpha 1";

int main(int argc, char **argv)
{
    KAboutData about("lintvscheduler", 0, ki18n("LinTV Scheduler"), version,
                     ki18n(description), KAboutData::License_GPL, 
                     ki18n("(C) 2009 Zamora Studios"), KLocalizedString(), 0, 
                     "xtremek2009@gmail.com");
    about.addAuthor(ki18n("Jesse L. Zamora"), KLocalizedString(), "xtremek2009@gmail.com");
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("display <display>", ki18n("The display on which to run LinTV Scheduler (e.g. --display :0)"));
    KCmdLineArgs::addCmdLineOptions(options);

    KUniqueApplication::addCmdLineOptions();
    KUniqueApplication *app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("display")) {
        Display *disp;
        disp = XOpenDisplay(args->getOption("display").toUtf8());
        app = new KUniqueApplication(disp);
    } else {
        app = new KUniqueApplication;
    }

    LinTVScheduler *widget = new LinTVScheduler;

    if (app->isSessionRestored()) {
        RESTORE(LinTVScheduler);
    } else {

        if (args->count() == 0) {
            widget->show();
        } else {
            int i = 0;
            for (; i < args->count(); i++) {
                widget->show();
            }
        }
        args->clear();
    }


    return app->exec();
}