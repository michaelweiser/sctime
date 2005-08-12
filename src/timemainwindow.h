/*
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

#ifndef TIMEMAINWINDOW_H
#define TIMEMAINWINDOW_H

#include <q3listview.h>
#include <q3mainwindow.h>
#include <Q3MimeSourceFactory>
#include <Q3Action>
//Added by qt3to4:
#include <QCustomEvent>
#include "kontodateninfo.h"
#include "unterkontodialog.h"
#include "kontotreeview.h"
#include "toolbar.h"
#include "q3toolbar.h"
#include "sctimexmlsettings.h"
#include "defaultcommentreader.h"

#define SIGINT_EVENT_ID QEvent::User


class StatusBar;

/** Diese Klasse implementiert das Hauptfenster des Programms,
    und sorgt zudem fuer das Fortschreiten der Zeit.
*/
class TimeMainWindow: public Q3MainWindow
{
  Q_OBJECT

  public:
    TimeMainWindow(KontoDatenInfo* zk);
    Q3ListView* getKontoTree() { return kontoTree; };
    virtual ~TimeMainWindow();
    SCTimeXMLSettings* settings;

  public slots:

    void callUnterKontoDialog(Q3ListViewItem * item);

    void callDateDialog();

    void callAboutBox();

    void minutenUhr();

    void pause();

    void pauseAbzur(bool on);

    void zeitChanged();

    void updateCaption();

    void save();

    void resetDiff();
    void checkIn();

    void inPersoenlicheKonten(bool hinzufuegen);
    void flagsChanged(const QString& abt, const QString& ko, const QString& uko, int idx);
    void changeShortCutSettings(Q3ListViewItem * item);

    void editUnterKontoPressed();
    void changeDate(const QDate& datum);
    void setAktivesProjekt(Q3ListViewItem * item);
    void showAdditionalButtons(bool show);
    void eintragAktivieren();
    void eintragHinzufuegen();
    void eintragEntfernen();
    void addDeltaToZeit(int delta, bool abzurOnly=false);
    void addTimeInc();
    void subTimeInc();
    void addFastTimeInc();
    void subFastTimeInc();
    void addAbzurTimeInc();
    void subAbzurTimeInc();
    void addFastAbzurTimeInc();
    void subFastAbzurTimeInc();

    void callFindKontoDialog();
    void callHelpDialog();
    void callPreferenceDialog();
    void refreshKontoListe();
    void reloadDefaultComments();
    void configClickMode(bool singleClickActivation);
    void mouseButtonInKontoTreeClicked(int button, Q3ListViewItem * item, const QPoint & pos, int c );
    void copyNameToClipboard();

  signals:
    /** Wird ausgeloest, falls sich die Gesamtzeit geaendert hat. Uebergeben wird die neue Gesamtzahl der Sekunden. */
    void gesamtZeitChanged(int) ;

    /** Wird ausgeloest, falls sich die abzurechnende Gesamtzeit
      * geaendert hat. Uebergeben wird die neue Gesamtzahl der Sekunden.
      */
    void gesamtZeitAbzurChanged(int) ;

    /**
      * Wird minuetlich ausgeloest, falls keine Pause aktiv ist.
      */
    void minuteTick();

    /**
      * Wird minuetlich ausgeloest, falls keine Pause aktiv ist.
      */
    void minuteAbzurTick();

    /**
     * Wird mit true ausgeloest, wenn ein Eintrag im Kontobaum
     * selektiert wurde, bei anderen Selektionen (Konto,Abteilung oder
     * Unterkonto mit mehreren Eintraegen) mit false
     */
    void eintragSelected(bool isEintrag);

    /** Wird mit true ausgeloest, falls auf das aktuelle Datum gewechselt wird, bei allen anderen
        Datumswechseln mit false. */
    void currentDateSelected(bool);

    /**
     * Wird mit true ausgeloest, wenn ein Eintrag im Kontobaum
     * selektiert wurde, der aktivierbar ist, bei anderen Selektionen (Konto,Abteilung oder
     * Unterkonto mit mehreren Eintraegen) mit false
     */
    void aktivierbarerEintragSelected(bool isActivable);

  private:
    KontoTreeView* kontoTree;
    UnterKontoDialog* unterKontoDialog;
    Q3Action* editUnterKontoAction;
    Q3Action* inPersKontAction;
    Q3Action* abzurMin5PlusAction;
    Q3Action* abzurMin5MinusAction;
    Q3Action* fastAbzurPlusAction;
    Q3Action* fastAbzurMinusAction;
    Q3Action* eintragRemoveAction;
    Q3Action* checkInAction;
    AbteilungsListe* abtList;
    AbteilungsListe* abtListToday;
    StatusBar* statusBar;
    Q3MimeSourceFactory* mimeSourceFactory;
    DefaultCommentReader* defaultCommentReader;
    Q3ToolBar* powerToolBar;
    ToolBar* toolBar;
    QStringList defaultTags;
    bool paused;
    bool pausedAbzur;

    // Workaround, um beim Setzen der Voreinstellung fuer den inPersoenlicheKonten-Button nicht das zugehoerige
    // Event auzuloesen. Wenn inPersoenlicheKontenAllowed=false, tut inPersoenlicheKonten(bool) gar nichts.
    bool inPersoenlicheKontenAllowed;
};

#endif
