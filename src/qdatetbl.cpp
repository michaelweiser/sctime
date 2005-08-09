/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2003 Florian Schmitt <f.schmitt@science-computing.de>
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

// This file has been ported from KDE to plain QT

/////////////////// KDateTable widget class //////////////////////
//
// Copyright (C) 1997 Tim D. Gilman
//           (C) 1998-2001 Mirko Boehm
// Written using Qt (http://www.troll.no) for the
// KDE project (http://www.kde.org)
//
// This is a support class for the KDatePicker class.  It just
// draws the calender table without titles, but could theoretically
// be used as a standalone.
//
// When a date is selected by the user, it emits a signal:
//      dateSelected(QDate)

#include <qapplication.h>
#include <qdesktopwidget.h>
#include "qcalendarsystem.h"

#include "qdatepicker.h"
#include "qdatetbl.h"

#include <q3popupmenu.h>

#include <qdatetime.h>
#include <qstring.h>
#include <qpen.h>
#include <qpainter.h>
#include <qdialog.h>
#include <q3dict.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <Q3Frame>
#include <QKeyEvent>
#include <QEvent>
#include <QWheelEvent>
#include <assert.h>

class QDateTable::QDateTablePrivate
{
public:
   QDateTablePrivate()
   {
      popupMenuEnabled=false;
      useCustomColors=false;
   }

   ~QDateTablePrivate()
   {
   }

   bool popupMenuEnabled;
   bool useCustomColors;

   struct DatePaintingMode
   {
     QColor fgColor;
     QColor bgColor;
     BackgroundMode bgMode;
   };
   Q3Dict <DatePaintingMode> customPaintingModes;

};


QDateValidator::QDateValidator(QWidget* parent, const char* name)
    : QValidator(parent, name)
{
}

QValidator::State
QDateValidator::validate(QString& text, int&) const
{
  QDate temp;
  // ----- everything is tested in date():
  return date(text, temp);
}

QValidator::State
QDateValidator::date(const QString& text, QDate& d) const
{
  QDate tmp = QDate::fromString(text,Qt::ISODate);
  if (!tmp.isNull())
    {
      d = tmp;
      return Acceptable;
    } else
      return QValidator::Intermediate;
}

void
QDateValidator::fixup( QString& ) const
{

}

QDateTable::QDateTable(QCalendarSystem* calendar_, QWidget *parent, QDate date_, const char* name, Qt::WFlags f)
  : Q3GridView(parent, name, f)
{
  d = new QDateTablePrivate;
  calendar=calendar_;
  setFontSize(10);
  if(!date_.isValid())
    {
      //kdDebug() << "QDateTable ctor: WARNING: Given date is invalid, using current date." << endl;
      date_=QDate::currentDate();
    }
  setFocusPolicy( Qt::StrongFocus );
  setNumRows(7); // 6 weeks max + headline
  setNumCols(7); // 7 days a week
  setHScrollBarMode(AlwaysOff);
  setVScrollBarMode(AlwaysOff);

  //viewport()->setEraseColor(KGlobalSettings::baseColor());

  setDate(date_); // this initializes firstday, numdays, numDaysPrevMonth
}

QDateTable::~QDateTable()
{
  delete d;
}

int QDateTable::posFromDate( const QDate &dt )
{
  const int firstWeekDay = 1;
  int pos = calendar->day( dt );
  int offset = (firstday - firstWeekDay + 7) % 7;
  // make sure at least one day of the previous month is visible.
  // adjust this <1 if more days should be forced visible:
  if ( offset < 1 ) offset += 7;
  return pos + offset;
}

QDate QDateTable::dateFromPos( int pos )
{
  QDate pCellDate;
  calendar->setYMD(pCellDate, calendar->year(date), calendar->month(date), 1);

  int firstWeekDay = 1;
  int offset = (firstday - firstWeekDay + 7) % 7;
  // make sure at least one day of the previous month is visible.
  // adjust this <1 if more days should be forced visible:
  if ( offset < 1 ) offset += 7;
  pCellDate = calendar->addDays( pCellDate, pos - offset );
  return pCellDate;
}

void
QDateTable::paintCell(QPainter *painter, int row, int col)
{
  QRect rect;
  QString text;
  QPen pen;
  int w=cellWidth();
  int h=cellHeight();
  QBrush brushBlue(Qt::blue);
  QBrush brushLightblue(Qt::white);
  QFont font;
  // -----

  if(row==0)
    { // we are drawing the headline
      font.setBold(true);
      painter->setFont(font);
      bool normalday = true;
      int firstWeekDay = 1;
      int daynum = ( col+firstWeekDay < 8 ) ? col+firstWeekDay :
                                              col+firstWeekDay-7;
      if ( daynum == calendar->weekDayOfPray() ||
         ( daynum == 6 && calendar->calendarName() == "gregorian" ) )
          normalday=false;

      if (!normalday)
        {
          painter->setPen(Qt::white);
          painter->setBrush(brushLightblue);
          painter->drawRect(0, 0, w, h);
          painter->setPen(Qt::blue);
        } else {
          painter->setPen(Qt::blue);
          painter->setBrush(brushBlue);
          painter->drawRect(0, 0, w, h);
          painter->setPen(Qt::white);
        }
      painter->drawText(0, 0, w, h-1, Qt::AlignCenter,
                        calendar->weekDayName(daynum, true), -1, &rect);
      painter->setPen(Qt::black);
      painter->drawLine(0, h-1, w-1, h-1);
      // ----- draw the weekday:
    } else {
      bool paintRect=true;
      painter->setFont(font);
      int pos=7*(row-1)+col;

      QDate pCellDate = dateFromPos( pos );
      // First day of month
      text = calendar->dayString(pCellDate, true);
      if( calendar->month(pCellDate) != calendar->month(date) )
        { // we are either
          // � painting a day of the previous month or
          // � painting a day of the following month
          painter->setPen(Qt::gray);
        } else { // paint a day of the current month
          if ( d->useCustomColors )
          {
            QDateTablePrivate::DatePaintingMode *mode=d->customPaintingModes[pCellDate.toString(Qt::ISODate)];
            if (mode)
            {              
              if (mode->bgMode != NoBgMode)
              {
                QBrush oldbrush=painter->brush();
                painter->setBrush( mode->bgColor );
                switch(mode->bgMode)
                {
                  case(CircleMode) : painter->drawEllipse(0,0,w,h);break;
                  case(RectangleMode) : painter->drawRect(0,0,w,h);break;
                  case(NoBgMode) : // Should never be here, but just to get one
                                   // less warning when compiling
                  default: break;
                }
                painter->setBrush( oldbrush );
                paintRect=false;
              }
              painter->setPen( mode->fgColor );
            } else
              painter->setPen(Qt::black);
          } else //if ( firstWeekDay < 4 ) // <- this doesn' make sense at all!
          painter->setPen(Qt::black);
        }

      pen=painter->pen();
      int firstWeekDay=1;
      int offset=firstday-firstWeekDay;
      if(offset<1)
        offset+=7;
      int d = calendar->day(date);
      if( (offset+d) == (pos+1) )
        {
          if(hasFocus())
            { // draw the currently selected date
              painter->setPen(Qt::blue);
              painter->setBrush(Qt::blue);
              pen=QPen(Qt::white);
            } else {
              painter->setPen(Qt::gray);
              painter->setBrush(Qt::gray);
              pen=QPen(Qt::white);
            }
        } else {
          painter->setBrush(Qt::white);
          painter->setPen(Qt::white);
        }

      if ( pCellDate == QDate::currentDate() )
      {
         painter->setPen(Qt::black);
      }

      if ( paintRect ) painter->drawRect(0, 0, w, h);
      painter->setPen(pen);
      painter->drawText(0, 0, w, h, Qt::AlignCenter, text, -1, &rect);
    }
  if(rect.width()>maxCell.width()) maxCell.setWidth(rect.width());
  if(rect.height()>maxCell.height()) maxCell.setHeight(rect.height());
}

void
QDateTable::keyPressEvent( QKeyEvent *e )
{
    QDate temp = date;

    switch( e->key() ) {
    case Qt::Key_PageUp:
        temp = calendar->addMonths( date, -1 );
        setDate(temp);
        return;
    case Qt::Key_PageDown:
        temp = calendar->addMonths( date, 1 );
        setDate(temp);
        return;
    case Qt::Key_Up:
        if ( calendar->day(date) > 7 ) {
            setDate(date.addDays(-7));
            return;
        }
        break;
    case Qt::Key_Down:
        if ( calendar->day(date) <= calendar->daysInMonth(date)-7 ) {
            setDate(date.addDays(7));
            return;
        }
        break;
    case Qt::Key_Left:
        if ( calendar->day(date) > 1 ) {
            setDate(date.addDays(-1));
            return;
        }
        break;
    case Qt::Key_Right:
        if ( calendar->day(date) < calendar->daysInMonth(date) ) {
            setDate(date.addDays(1));
            return;
        }
        break;
    case Qt::Key_Minus:
        setDate(date.addDays(-1));
        return;
    case Qt::Key_Plus:
        setDate(date.addDays(1));
        return;
    case Qt::Key_N:
        setDate(QDate::currentDate());
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit tableClicked();
        return;
    default:
        break;
    }
}

void
QDateTable::viewportResizeEvent(QResizeEvent * e)
{
  Q3GridView::viewportResizeEvent(e);

  setCellWidth(viewport()->width()/7);
  setCellHeight(viewport()->height()/7);
}

void
QDateTable::setFontSize(int size)
{
  int count;
  QFontMetrics metrics(fontMetrics());
  QRect rect;
  // ----- store rectangles:
  fontsize=size;
  // ----- find largest day name:
  maxCell.setWidth(0);
  maxCell.setHeight(0);
  for(count=0; count<7; ++count)
    {
      rect=metrics.boundingRect(calendar->weekDayName(count+1, true));
      maxCell.setWidth(QMAX(maxCell.width(), rect.width()));
      maxCell.setHeight(QMAX(maxCell.height(), rect.height()));
    }
  // ----- compare with a real wide number and add some space:
  rect=metrics.boundingRect(QString::fromLatin1("88"));
  maxCell.setWidth(QMAX(maxCell.width()+2, rect.width()));
  maxCell.setHeight(QMAX(maxCell.height()+4, rect.height()));
}

void
QDateTable::wheelEvent ( QWheelEvent * e )
{
    setDate(date.addMonths( -(int)(e->delta()/120)) );
    e->accept();
}

void
QDateTable::contentsMousePressEvent(QMouseEvent *e)
{

  if ((e->type()!=QEvent::MouseButtonPress)&&(e->type()!=QEvent::MouseButtonDblClick))
    { // the KDatePicker only reacts on mouse press events:
      return;
    }

  // -----
  int row, col, pos, temp;
  QPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());
  if(row<1 || col<0)
    { // the user clicked on the frame of the table
      return;
    }

  // Rows and columns are zero indexed.  The (row - 1) below is to avoid counting
  // the row with the days of the week in the calculation.

  // old selected date:
  temp = posFromDate( date );
  // new position and date
  pos = (7 * (row - 1)) + col;
  QDate clickedDate = dateFromPos( pos );

  // set the new date. If it is in the previous or next month, the month will
  // automatically be changed, no need to do that manually...
  setDate( clickedDate );

  // call updateCell on the old and new selection. If setDate switched to a different
  // month, these cells will be painted twice, but that's no problem.
  updateCell( temp/7+1, temp%7 );
  updateCell( row, col );
  if (e->type()==QEvent::MouseButtonDblClick)
    emit tableDoubleClicked();
  else
    emit tableClicked();

  if (  e->button() == Qt::RightButton && d->popupMenuEnabled )
  {
        Q3PopupMenu *menu = new Q3PopupMenu();
        //menu->insertTitle( KGlobal::locale()->formatDate(clickedDate) );
        emit aboutToShowContextMenu( menu, clickedDate );
        menu->popup(e->globalPos());
  }
}

void
QDateTable::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
  contentsMousePressEvent(e);
}

bool
QDateTable::setDate(const QDate& date_)
{
  bool changed=false;
  QDate temp;
  // -----
  if(!date_.isValid())
    {
      return false;
    }
  if(date!=date_)
    {
      emit(dateChanged(date, date_));
      date=date_;
      emit(dateChanged(date));
      changed=true;
    }

  calendar->setYMD(temp, calendar->year(date), calendar->month(date), 1);
  //temp.setYMD(date.year(), date.month(), 1);
  //kdDebug() << "firstDayInWeek: " << temp.toString() << endl;
  firstday=temp.dayOfWeek();
  numdays=calendar->daysInMonth(date);

  temp = calendar->addMonths(temp, -1);
  numDaysPrevMonth=calendar->daysInMonth(temp);
  if(changed)
    {
      repaintContents(false);
    }
  return true;
}

const QDate&
QDateTable::getDate() const
{
  return date;
}

// what are those repaintContents() good for? (pfeiffer)
void QDateTable::focusInEvent( QFocusEvent *e )
{
//    repaintContents(false);
    Q3GridView::focusInEvent( e );
}

void QDateTable::focusOutEvent( QFocusEvent *e )
{
//    repaintContents(false);
    Q3GridView::focusOutEvent( e );
}

QSize
QDateTable::sizeHint() const
{
  if(maxCell.height()>0 && maxCell.width()>0)
    {
      return QSize(maxCell.width()*numCols()+2*frameWidth(),
             (maxCell.height()+2)*numRows()+2*frameWidth());
    } else {
      return QSize(-1, -1);
    }
}

void QDateTable::setPopupMenuEnabled( bool enable )
{
   d->popupMenuEnabled=enable;
}

bool QDateTable::popupMenuEnabled() const
{
   return d->popupMenuEnabled;
}

void QDateTable::setCustomDatePainting(const QDate &date, const QColor &fgColor, BackgroundMode bgMode, const QColor &bgColor)
{
    if (!fgColor.isValid())
    {
        unsetCustomDatePainting( date );
        return;
    }

    QDateTablePrivate::DatePaintingMode *mode=new QDateTablePrivate::DatePaintingMode;
    mode->bgMode=bgMode;
    mode->fgColor=fgColor;
    mode->bgColor=bgColor;

    d->customPaintingModes.replace( date.toString(Qt::ISODate), mode );
    d->useCustomColors=true;
    update();
}

void QDateTable::unsetCustomDatePainting( const QDate &date )
{
    d->customPaintingModes.remove( date.toString(Qt::ISODate) );
}

QDateInternalWeekSelector::QDateInternalWeekSelector
(QWidget* parent, const char* name)
  : QLineEdit(parent, name),
    val(new QIntValidator(this)),
    result(0)
{
  QFont font;
  // -----
  setFont(font);
  setFrame(false);
  setValidator(val);
  connect(this, SIGNAL(returnPressed()), SLOT(weekEnteredSlot()));
}

void
QDateInternalWeekSelector::weekEnteredSlot()
{
  bool ok;
  int week;
  // ----- check if this is a valid week:
  week=text().toInt(&ok);
  if(!ok)
    {
      return;
    }
  result=week;
  emit(closeMe(1));
}

int
QDateInternalWeekSelector::getWeek()
{
  return result;
}

void
QDateInternalWeekSelector::setWeek(int week)
{
  QString temp;
  // -----
  temp.setNum(week);
  setText(temp);
}

void
QDateInternalWeekSelector::setMaxWeek(int max)
{
  val->setRange(1, max);
}

// ### CFM To avoid binary incompatibility.
//     In future releases, remove this and replace by  a QDate
//     private member, needed in KDateInternalMonthPicker::paintCell
class QDateInternalMonthPicker::QDateInternalMonthPrivate {
public:
        QDateInternalMonthPrivate (int y, int m, int d)
        : year(y), month(m), day(d)
        {};
        int year;
        int month;
        int day;
};

QDateInternalMonthPicker::~QDateInternalMonthPicker() {
   delete d;
}

QDateInternalMonthPicker::QDateInternalMonthPicker
(QCalendarSystem* calendar_, const QDate & date, QWidget* parent, const char* name)
  : Q3GridView(parent, name),
    result(0) // invalid
{
  QRect rect;
  QFont font;
  // -----
  calendar = calendar_;
  activeCol = -1;
  activeRow = -1;
  setFont(font);
  setHScrollBarMode(AlwaysOff);
  setVScrollBarMode(AlwaysOff);
  setFrameStyle(Q3Frame::NoFrame);
  setNumCols(3);
  d = new QDateInternalMonthPrivate(date.year(), date.month(), date.day());
  // For monthsInYear != 12
  setNumRows( (calendar->monthsInYear(date) + 2) / 3);
  // enable to find drawing failures:
  // setTableFlags(Tbl_clipCellPainting);

  //viewport()->setEraseColor(KGlobalSettings::baseColor()); // for consistency with the datepicker

  // ----- find the preferred size
  //       (this is slow, possibly, but unfortunately it is needed here):
  QFontMetrics metrics(font);
  for(int i = 1; ; ++i)
    {
      QString str = calendar->monthName(i,
         calendar->year(date), false);
      if (str.isNull()) break;
      rect=metrics.boundingRect(str);
      if(max.width()<rect.width()) max.setWidth(rect.width());
      if(max.height()<rect.height()) max.setHeight(rect.height());
    }
}

QSize
QDateInternalMonthPicker::sizeHint() const
{
  return QSize((max.width()+6)*numCols()+2*frameWidth(),
         (max.height()+6)*numRows()+2*frameWidth());
}

int
QDateInternalMonthPicker::getResult() const
{
  return result;
}

void
QDateInternalMonthPicker::setupPainter(QPainter *p)
{
  p->setPen(Qt::black);
}

void
QDateInternalMonthPicker::viewportResizeEvent(QResizeEvent*)
{
  setCellWidth(width() / numCols());
  setCellHeight(height() / numRows());
}

void
QDateInternalMonthPicker::paintCell(QPainter* painter, int row, int col)
{
  int index;
  QString text;
  // ----- find the number of the cell:
  index=3*row+col+1;
  text=calendar->monthName(index,
    calendar->year(QDate(d->year, d->month,
    d->day)), false);
  painter->drawText(0, 0, cellWidth(), cellHeight(), Qt::AlignCenter, text);
  if ( activeCol == col && activeRow == row )
      painter->drawRect( 0, 0, cellWidth(), cellHeight() );
}

void
QDateInternalMonthPicker::contentsMousePressEvent(QMouseEvent *e)
{
  if(!isEnabled() || e->button() != Qt::LeftButton)
    {
      return;
    }
  // -----
  int row, col;
  QPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());

  if(row<0 || col<0)
    { // the user clicked on the frame of the table
      activeCol = -1;
      activeRow = -1;
    } else {
      activeCol = col;
      activeRow = row;
      updateCell( row, col /*, false */ );
  }
}

void
QDateInternalMonthPicker::contentsMouseMoveEvent(QMouseEvent *e)
{
  if (e->state() & Qt::LeftButton)
    {
      int row, col;
      QPoint mouseCoord;
      // -----
      mouseCoord = e->pos();
      row=rowAt(mouseCoord.y());
      col=columnAt(mouseCoord.x());
      int tmpRow = -1, tmpCol = -1;
      if(row<0 || col<0)
        { // the user clicked on the frame of the table
          if ( activeCol > -1 )
            {
              tmpRow = activeRow;
              tmpCol = activeCol;
            }
          activeCol = -1;
          activeRow = -1;
        } else {
          bool differentCell = (activeRow != row || activeCol != col);
          if ( activeCol > -1 && differentCell)
            {
              tmpRow = activeRow;
              tmpCol = activeCol;
            }
          if ( differentCell)
            {
              activeRow = row;
              activeCol = col;
              updateCell( row, col /*, false */ ); // mark the new active cell
            }
        }
      if ( tmpRow > -1 ) // repaint the former active cell
          updateCell( tmpRow, tmpCol /*, true */ );
    }
}

void
QDateInternalMonthPicker::contentsMouseReleaseEvent(QMouseEvent *e)
{
  if(!isEnabled())
    {
      return;
    }
  // -----
  int row, col, pos;
  QPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());
  if(row<0 || col<0)
    { // the user clicked on the frame of the table
      emit(closeMe(0));
    }

  pos=3*row+col+1;
  result=pos;
  emit(closeMe(1));
}



QDateInternalYearSelector::QDateInternalYearSelector
(QCalendarSystem* calendar_, QWidget* parent, const char* name)
  : QLineEdit(parent, name),
    val(new QIntValidator(this)),
    result(0)
{
  QFont font;
  calendar=calendar_;
  // -----
  setFont(font);
  setFrame(false);
  // we have to respect the limits of QDate here, I fear:
  val->setRange(0, 8000);
  setValidator(val);
  connect(this, SIGNAL(returnPressed()), SLOT(yearEnteredSlot()));
}

void
QDateInternalYearSelector::yearEnteredSlot()
{
  bool ok;
  int year;
  QDate date;
  // ----- check if this is a valid year:
  year=text().toInt(&ok);
  if(!ok)
    {
      return;
    }
  //date.setYMD(year, 1, 1);
  calendar->setYMD(date, year, 1, 1);
  if(!date.isValid())
    {
      return;
    }
  result=year;
  emit(closeMe(1));
}

int
QDateInternalYearSelector::getYear()
{
  return result;
}

void
QDateInternalYearSelector::setYear(int year)
{
  QString temp;
  // -----
  temp.setNum(year);
  setText(temp);
}

QPopupFrame::QPopupFrame(QWidget* parent, const char*  name)
  : Q3Frame(parent, name, Qt::WType_Popup),
    result(0), // rejected
    main(0)
{
  setFrameStyle(Q3Frame::Box|Q3Frame::Raised);
  setMidLineWidth(2);
}

void
QPopupFrame::keyPressEvent(QKeyEvent* e)
{
  if(e->key()==Qt::Key_Escape)
    {
      result=0; // rejected
      qApp->exit_loop();
    }
}

void
QPopupFrame::close(int r)
{
  result=r;
  qApp->exit_loop();
}

void
QPopupFrame::setMainWidget(QWidget* m)
{
  main=m;
  if(main!=0)
    {
      resize(main->width()+2*frameWidth(), main->height()+2*frameWidth());
    }
}

void
QPopupFrame::resizeEvent(QResizeEvent*)
{
  if(main!=0)
    {
      main->setGeometry(frameWidth(), frameWidth(),
          width()-2*frameWidth(), height()-2*frameWidth());
    }
}

void
QPopupFrame::popup(const QPoint &pos)
{
  // Make sure the whole popup is visible.
  QDesktopWidget qdw;
  QRect d = qdw.screenGeometry(pos);
  int x = pos.x();
  int y = pos.y();
  int w = width();
  int h = height();
  if (x+w > d.x()+d.width())
    x = d.width() - w;
  if (y+h > d.y()+d.height())
    y = d.height() - h;
  if (x < d.x())
    x = 0;
  if (y < d.y())
    y = 0;

  // Pop the thingy up.
  move(x, y);
  show();
}

int
QPopupFrame::exec(QPoint pos)
{
  popup(pos);
  repaint();
  qApp->enter_loop();
  hide();
  return result;
}

int
QPopupFrame::exec(int x, int y)
{
  return exec(QPoint(x, y));
}

void QPopupFrame::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data ); */}

void QDateTable::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

