/*
    $Id$

    Copyright (C) 2003 Florian Schmitt, Science and Computing AG
                       f.schmitt@science-computing.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <QTextCodec>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QString>
#include <QTranslator>
#include <QSqlDatabase>

#ifdef WIN32
#include <windows.h>
#else
#include <assert.h>
#include <locale.h>
#include <signal.h>
#include "unix/signalhandler.h"
#define LOCALE "de_DE.UTF-8"
#endif

#include "GetOpt.h"
#include "lock.h"
#include "timemainwindow.h"
#include "sctimexmlsettings.h"
#include "globals.h"
#include "kontotreeview.h"
#include "datasource.h"
#include "setupdsm.h"

#ifndef CONFIGDIR
#define CONFIGDIR "~/.sctime"
#endif


// transform the value a #define into a string in a portable way
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)

#ifdef __GNUC__
static void fatal(const QString& title, const QString& body) __attribute__ ((noreturn));
#else
static void fatal(const QString& title, const QString& body);
#endif

QDir configDir;
QString lockfilePath;
QString PERSOENLICHE_KONTEN_STRING;
QString ALLE_KONTEN_STRING;
Lock *lock;

static void fatal(const QString& title, const QString& body) {
  QMessageBox::critical(NULL, title, body, QMessageBox::Ok);
  exit(1);
}

static const QString help(QObject::tr(
"Available Options:\n"
"--configdir=DIR	location of the directory where your files will be placed\n"
"			(default: ~/.sctime)\n"
"--datasource=		(repeatable) use these data sources\n"
"			(default: 'QPSQL'',' 'QODBC'', 'command' and 'file'');\n"
"			overrides <backends/> in settings.xml\n"
"--zeitkontenfile=FILE	read the accounts list from file FILE\n"
"			(default: output of 'zeitkonten'.\n\n"
"--bereitschaftsfile=FILE	read the 'Bereitschaftsarten'' from file FILE\n"
"				(default: output of 'zeitbereitls'.\n\n"
"Please see the Help menu for further information (F1)!"));

static TimeMainWindow* mainWindow = 0;

#ifdef WIN32
class SctimeApp : public QApplication {
public:
    SctimeApp(int &argc, char **argv):QApplication(argc, argv) {}
    virtual bool SctimeApp::winEventFilter(MSG * msg, long * result) {
      if (msg->message == WM_POWERBROADCAST && mainWindow && msg->hwnd == mainWindow->winId()) {
        if (msg->wParam == PBT_APMRESUMEAUTOMATIC)
            QMetaObject::invokeMethod(mainWindow, "resume", Qt::QueuedConnection);
        else if (msg->wParam == PBT_APMSUSPEND)
            QMetaObject::invokeMethod(mainWindow, "suspend", Qt::QueuedConnection);
        else return false;
	*result = TRUE;
	return true;
      }
      return false;
    }
};
#else
#define SctimeApp QApplication
#endif

QString canonicalPath(QString path) {
    if (path == "~" || path.startsWith("~/") || path.startsWith(QString("~") +
		QDir::separator())) {
#ifdef WIN32
	QString homedir = "H:";
	// append a separator if only the homedir is requested so that the
	// drive's root is addressed reliably
	if (path == "~")
		homedir.append(QDir::separator());
#else
	QString homedir = QDir::homePath();
#endif /* WIN32 */

	return path.replace(0, 1, homedir);
    }
    return QFileInfo(path).canonicalFilePath();
}


/** main: hier wird ueberprueft, ob die Applikation ueberhaupt starten soll
 * (Lockfiles,...), und falls ja, wird SCTimeApp initialisiert und
 ausgefuehrt */
int main(int argc, char **argv ) {
  //QApplication app(argc, argv);
  SctimeApp app(argc, argv);
  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
          QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&qtTranslator);
  QTranslator sctimeTranslator;
  sctimeTranslator.load(":/translations/sctime");
  app.installTranslator(&sctimeTranslator);
  QTextCodec::setCodecForTr(QTextCodec::codecForName ("UTF-8"));
  app.setObjectName("sctime");
  app.setApplicationVersion(QUOTE(APP_VERSION));

  PERSOENLICHE_KONTEN_STRING = QObject::tr("Personal accounts");
  ALLE_KONTEN_STRING = QObject::tr("All accounts");

  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1],"--help") == 0 || strcmp(argv[1], "/h") == 0||strcmp(argv[1],"/help") == 0)) {
    QMessageBox::information(NULL, QObject::tr("sctime ") + qApp->applicationVersion(), help);
    exit(0);
  }
  QString configdirstring, zeitkontenfile, bereitschaftsfile;
  QStringList dataSourceNames;

  GetOpt opts(argc, argv);
  opts.addOption('f',"configdir", &configdirstring);
  opts.addOption('f',"zeitkontenfile", &zeitkontenfile);
  opts.addOption('f',"bereitschaftsfile", &bereitschaftsfile);
  opts.addRepeatableOption("datasource", &dataSourceNames);
  opts.parse();

  if (configdirstring.isEmpty()) {
    char *envpointer = getenv("SCTIME_CONFIG_DIR");
    configdirstring = envpointer ? envpointer : CONFIGDIR; // default Configdir
  }

  // configdirstring can no longer be empty now but still may be relative or
  // reference home dir by ~
  configdirstring = canonicalPath(configdirstring);
  QDir directory;
  if (!directory.cd(configdirstring)) {
    directory.mkdir(configdirstring);
    if (!directory.cd(configdirstring))
      fatal(QObject::tr("sctime: Configuration problem"),
        QObject::tr("Cannot access configration directory %1.").arg(configdirstring));
  }
  configDir=directory.path();
  // Ich lege eine lokale Sperre an, die vom Betriebssystem zuverlässig auch
  // nach einem Absturz aufgegeben wird. Außerdem lege ich noch ein globales
  // Lock mit dem Rechnernamen an.
  // Gründe:
  // - Es gibt keine globalen Sperren (für SMB und NFS hinweg sichtbar), die
  //   nach einem Absturz automatisch entfernt werden.
  // - Das Programm kann ausgeben, auf welchem Rechner sctime noch läuft.
  // - Nach einem Absturz kann ich auf dem gleichen Rechner neu starten,
  //   ohne de Benutzer mit Warnungen wegen alter Sperren zu belästigen.
  LockLocal local("sctime", true);
  Lock *global = new Lockfile(configDir.filePath("LOCK"), true);
  local.setNext(global);
  if (!local.acquire()) fatal(QObject::tr("sctime: Cannot start"), local.errorString());
  lock = &local;
  SCTimeXMLSettings settings;
  settings.readSettings();
  if (dataSourceNames.isEmpty()) dataSourceNames = settings.backends.split(" ");
  setupDatasources(dataSourceNames, settings, zeitkontenfile, bereitschaftsfile);
  mainWindow = new TimeMainWindow();
#ifndef WIN32
  SignalHandler term(SIGTERM);
  app.connect(&term, SIGNAL(received()), &app, SLOT(closeAllWindows()));
  SignalHandler hup(SIGHUP);
  app.connect(&hup, SIGNAL(received()), &app, SLOT(closeAllWindows()));
  SignalHandler int_(SIGINT);
  app.connect(&int_, SIGNAL(received()), &app, SLOT(closeAllWindows()));
  SignalHandler cont(SIGCONT);
  app.connect(&cont, SIGNAL(received()), mainWindow, SLOT(resume()));
#endif
  mainWindow->show();
  app.exec();
  delete mainWindow;
  local.release();
  delete global;
  delete kontenDSM;
  delete bereitDSM;
  return 0;
}
