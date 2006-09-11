# File generated by kdevelop's qmake manager.
# -------------------------------------------
# Subdir relative project main directory: ./src
# Target is an application:  ../bin/sctime

TEMPLATE = app
VERSIONSTR = \"0.53\"
CONFIG += warn_on qt
CONFIG += uic
OBJECTS_DIR = ../obj
QT += qt3support xml sql gui core
TARGET = ../bin/sctime
DEFINES += BUILDDATESTR=$$BUILDDATESTR VERSIONSTR=$$VERSIONSTR
SOURCES += abteilungsliste.cpp \
           datedialog.cpp \
           unterkontodialog.cpp \
           kontotreeview.cpp \
           findkontodialog.cpp \
           timemainwindow.cpp \
           sctimexmlsettings.cpp \
           sctime.cpp \
           preferencedialog.cpp \
           defaultcommentreader.cpp \
           qcalendarsystem.cpp \
           qcalendarsystemgregorian.cpp \
           qdatepicker.cpp \
           qdatetbl.cpp \
           utils.cpp \
           defaulttagreader.cpp \
           kontodateninfozeit.cpp \
           kontotreeitem.cpp \
           GetOpt.cpp \
           descdata.cpp \
           smallfontdialog.cpp \
           bereitschaftsdateninfozeit.cpp \
           bereitschaftsliste.cpp \
           bereitschaftsview.cpp
HEADERS += abteilungsliste.h \
           kontoliste.h \
           statusbar.h \
           datedialog.h \
           kontotreeitem.h \
           timecounter.h \
           eintragsliste.h \
           timeedit.h \
           errorapp.h \
           kontotreeview.h \
           findkontodialog.h \
           timemainwindow.h \
           globals.h \
           toolbar.h \
           sctimeapp.h \
           unterkontodialog.h \
           kontodateninfo.h \
           sctimehelp.h \
           unterkontoeintrag.h \
           sctimexmlsettings.h \
           unterkontoliste.h \
           preferencedialog.h \
           defaultcommentreader.h \
           utils.h \
           qcalendarsystem.h \
           qcalendarsystemgregorian.h \
           qdatepicker.h \
           qdatetbl.h \
           defaulttagreader.h \
           kontodateninfozeit.h \
           GetOpt.h \
           descdata.h \
           smallfontdialog.h \
           bereitschaftsdateninfozeit.h \
           bereitschaftsliste.h \
           bereitschaftsview.h
IMAGES += ../pics/hi16_action_apply.xpm \
          ../pics/hi22_action_1downarrow.xpm \
          ../pics/hi22_action_1uparrow.xpm \
          ../pics/hi22_action_2downarrow.xpm \
          ../pics/hi22_action_2uparrow.xpm \
          ../pics/hi22_action_attach.xpm \
          ../pics/hi22_action_edit.xpm \
          ../pics/hi22_action_filesave.xpm \
          ../pics/hi22_action_player_pause_half.xpm \
          ../pics/hi22_action_player_pause.xpm \
          ../pics/hi22_action_queue.xpm \
          ../pics/scLogo_15Farben.xpm \
          ../pics/sc_logo.xpm
FORMS += datedialogbase.ui \
         preferencedialogbase.ui
target.path = /$(prefix)/bin
INSTALLS += target
!win32{
  BUILDDATESTR = \""`date`"\"
}
isEmpty(BUILDDATESTR){
  BUILDDATESTR = \""unknown"\"
}
linux-g++-static{
  LIBS += -ldl
}
linux-g++-static-debug{
  LIBS += -ldl
}
linux-g++{
  LIBS += -ldl
}
win32{
  DEFINES += WIN32
  QMAKE_CXXFLAGS += -GX
  SOURCES += kontodateninfodatabase.cpp
  HEADERS += kontodateninfodatabase.h
}
hpux-acc{
  DEFINES += HPUX
  LIBS += -L/opt/graphics/OpenGL/lib $$QMAKE_LIBS_OPENGL
}
solaris-cc{
  DEFINES += SUN
  QMAKE_CXXFLAGS_RELEASE += -features=conststrings
  LIBS += -ldl $$QMAKE_LIBS_OPENGL
}
solaris-g++{
  DEFINES += SUN
  LIBS += -ldl
}
irix-cc{
  DEFINES += IRIX
  QMAKE_LIBS_QT += -lGL
}
irix-g++{
  DEFINES += IRIX
}
