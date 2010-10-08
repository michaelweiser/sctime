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

#include "kontodateninfodatabase.h"
#include "unterkontoeintrag.h"
#include <iostream>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QApplication>
#include <QVariant>
//Added by qt3to4:
#include <QSqlQuery>
#include "globals.h"
#include "descdata.h"
#include "DBConnector.h"

KontoDatenInfoDatabase::KontoDatenInfoDatabase(DBConnector* dbconnector)
{
  m_dbconnector=dbconnector;
}

/**
 * Liest aus einer ODBC-Datenbank nach abtList
 */
bool KontoDatenInfoDatabase::readInto(AbteilungsListe * abtList)
{
  bool ret = true;
  // PluginDir setzen
  QApplication::addLibraryPath(execDir+"/lib");
  abtList->clear();
  QSqlDatabase defaultDB = QSqlDatabase::addDatabase( "QODBC" );
  m_dbconnector->configureDB(defaultDB);

  if ( defaultDB.open() ) {
      QSqlQuery query(
              "SELECT gb.name,konto.name,unterkonto.name,unterkonto.beschreibung,v_tuser.name,unterkonto.art,mikro.kommentar "
              "FROM konto,gb,unterkonto "
              "LEFT JOIN v_tuser ON (v_tuser.user_id = unterkonto.verantwortlich) "
              "LEFT JOIN unterkonto_kommentar mikro ON (mikro.unterkonto_id = unterkonto.unterkonto_id) "
              "WHERE konto.konto_id = unterkonto.konto_id AND gb.gb_id = konto.gb_id AND unterkonto.eintragbar=\'y\';",
            defaultDB);
      if ( query.isActive() ) {
        while ( query.next() ) {
          QString abt = query.value(0).toString().simplified();
          QString ko = query.value(1).toString().simplified();
          QString uko = query.value(2).toString().simplified();
          QString beschreibung = query.value(3).toString();
          QString responsible = query.value(4).toString().simplified();;
          QString type = query.value(5).toString().simplified();;
          // Do not simplify comment to preserve intentional whitespace.
          QString commentstr = query.value(6).toString();
          abtList->insertEintrag(abt,ko,uko);
          if (beschreibung.isEmpty())
              beschreibung="";
          if (responsible.isEmpty())
              responsible="";
          if (type.isEmpty())
              type="";
          abtList->setDescription(abt,ko,uko,DescData(beschreibung,responsible,type));
          abtList->setUnterKontoFlags(abt,ko,uko,IS_IN_DATABASE,FLAG_MODE_OR);
          if ((!commentstr.isNull())&&(!commentstr.isEmpty())) {
                UnterKontoListe::iterator itUk;
                UnterKontoListe* ukl;
                if (abtList->findUnterKonto(itUk,ukl,abt,ko,uko)) {
                  itUk->second.addDefaultComment(commentstr);
                }
          }
        }
      }  else std::cout<<"Kann Abfrage nicht durchfuehren"<<std::endl;
  } else {
        ret = false;
#ifdef WIN32
                QMessageBox::critical(NULL,"Error","Kann Datenbank nicht �ffnen\n",
                              QMessageBox::Ok, Qt::NoButton,
                              Qt::NoButton);
#else
                std::cout<<"Kann Datenbank nicht �ffnen"<<std::endl;
#endif
    }

  defaultDB.close();
  return ret;
}

bool KontoDatenInfoDatabase::checkIn(AbteilungsListe* abtlist)
{
   std::cout<<"Kann nicht einchecken, da nicht implementiert"<<std::endl;
   return false;
}


/**
 * Liest aus einer ODBC-Datenbank nach abtList aber NUR Default Comments
 */
bool KontoDatenInfoDatabase::readDefaultCommentsInto(AbteilungsListe * abtList)
{
  bool ret = true;
  // PluginDir setzen
  //QApplication::addLibraryPath(execDir+"/lib");
  //abtList->clear();
  QSqlDatabase defaultDB = QSqlDatabase::addDatabase( "QODBC" );
  m_dbconnector = new DBConnector();
  m_dbconnector->configureDB(defaultDB);

  if ( defaultDB.open() ) {
      QSqlQuery query(
              "SELECT gb.name,konto.name,unterkonto.name,unterkonto.beschreibung,v_tuser.name,unterkonto.art,mikro.kommentar "
              "FROM konto,gb,unterkonto "
              "LEFT JOIN v_tuser ON (v_tuser.user_id = unterkonto.verantwortlich) "
              "LEFT JOIN unterkonto_kommentar mikro ON (mikro.unterkonto_id = unterkonto.unterkonto_id) "
              "WHERE konto.konto_id = unterkonto.konto_id AND gb.gb_id = konto.gb_id AND unterkonto.eintragbar=\'y\';",
            defaultDB);
      if ( query.isActive() ) {
        while ( query.next() ) {
          QString abt = query.value(0).toString().simplified();
          QString ko = query.value(1).toString().simplified();
          QString uko = query.value(2).toString().simplified();
          QString beschreibung = query.value(3).toString();
          QString responsible = query.value(4).toString().simplified();;
          QString type = query.value(5).toString().simplified();;
          // Do not simplify comment to preserve intentional whitespace.
          QString commentstr = query.value(6).toString();
          //abtList->insertEintrag(abt,ko,uko);
          if (beschreibung.isEmpty())
              beschreibung="";
          if (responsible.isEmpty())
              responsible="";
          if (type.isEmpty())
              type="";
          //abtList->setDescription(abt,ko,uko,DescData(beschreibung,responsible,type));
          //abtList->setUnterKontoFlags(abt,ko,uko,IS_IN_DATABASE,FLAG_MODE_OR);
          if ((!commentstr.isNull())&&(!commentstr.isEmpty())) {
                UnterKontoListe::iterator itUk;
                UnterKontoListe* ukl;
                if (abtList->findUnterKonto(itUk,ukl,abt,ko,uko)) {
                  itUk->second.addDefaultComment(commentstr);
                }
          }
        }
      }  else std::cout<<"Kann Abfrage nicht durchfuehren"<<std::endl;
  } else {
        ret = false;
//#ifdef WIN32
                //QMessageBox::critical(NULL,"Error","Kann Datenbank nicht �ffnen\n",
                              //QMessageBox::Ok, Qt::NoButton,
                              //Qt::NoButton);
//#else
                //std::cout<<"Kann Datenbank nicht �ffnen"<<std::endl;
//#endif
    }

  defaultDB.close();
  return ret;
}
