#include "lintv.h"
#include "lintvrecord.h"
#include "lintvplay.h"
#include "lintvdeviceinfo.h"
#include "lintvglobal.h"
#include "lintvsettings.h"
#include "lintvprocess.h"

#include <kapplication.h>
#include <kuniqueapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>
#include <kdebug.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <krun.h>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

#include <X11/Xlib.h>

static const char description[] =
    I18N_NOOP("A KDE 4 application for viewing\nand recording TV.");

static const char version[] = "0.1.2 Alpha 1";

int main(int argc, char **argv)
{
    KAboutData about("lintv", 0, ki18n("LinTV"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2009 Zamora Studios"), KLocalizedString(), 0, "xtremek2009@gmail.com");
    about.addAuthor( ki18n("Jesse L. Zamora"), KLocalizedString(), "xtremek2009@gmail.com" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("record <time>", ki18n("Starts recording in the background until <time> expires."), "5");
    options.add("filename <filename>", ki18n("Sets the name of the recording to <filename>"));
    options.add("input <input>", ki18n("Record from the <input> index"));
    options.add("device <device>", ki18n("The name of the device to record from."));
    options.add("channel <channel>", ki18n("The channel to record from."));
    KCmdLineArgs::addCmdLineOptions(options);
    
    bool useGui = false;

    // Look for a running X server. If none are found, we just record
    // without displaying a GUI....
    printf("Trying to find a running X server...\n");
    Display *disp;
    int i;
    char buf[20];
    for (i = 0; i < 10; i++) {
        sprintf(buf, ":%d.0", i);
        disp = XOpenDisplay(buf);
        if (disp != NULL) {
            printf("Found display: %d.0\n", i);
            useGui = true;
            XCloseDisplay(disp);
            break;
        }
    }

    // Set the DISPLAY environment variable if we are using a GUI
    // so LinTV can run succesfully from the job scheduler.
    if (useGui) {
        sprintf(buf, "DISPLAY=:%d.0", i);
        printf("Setting %s\n", buf);
        putenv(buf);
    } else {
        printf("No displays found.\n");
    }
              
    KApplication *app;

    // Intialize KApplication with or without a GUI...
    if (useGui) {
        app = new KUniqueApplication(true);
    } else {
        app = new KApplication(false);
    }

    // no session.. just start up normally
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    LinTV *lintv;

    // see if we are starting with session management
    if (app->isSessionRestored())
    {
        //RESTORE(LinTV);
    }
    else
    {
        if (args->isSet("record")) {
            if (!useGui) {
                printf("No displays were found, will record in the background...\n");
            }

            QString device;
            int recordTime;
            int input = 0;
            int channel = 3;
            QString filename;

            recordTime = args->getOption("record").toInt();

            if (args->isSet("device")) {
                device = args->getOption("device");
            }

            if (args->isSet("input")) {
                input = args->getOption("input").toInt();
            }

            if (args->isSet("channel")) {
                channel = args->getOption("channel").toInt();
            }

            if (args->isSet("filename")) {
                filename = args->getOption("filename");
            }

            lintv = new LinTV(useGui, device, input, channel, recordTime, filename);

            args->clear();
        } else {
            if (!useGui) {
                printf("Could not find a display to use."
                       " Will try running LinTV anyway...\n");
            }

            lintv = new LinTV;
            args->clear();
        }
    }

    return app->exec();
}
