# File generated by kdevelop's qmake manager.
# -------------------------------------------
# Subdir relative project main directory: ./src
# Target is an application:  ../bin/sctime

TEMPLATE = app
VERSIONSTR = '"0.70"'
CONFIG += warn_on qt uic
QT += xml gui core
TARGET = sctime
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
           qdatepicker.cpp \
           utils.cpp \
           defaulttagreader.cpp \
           kontodateninfozeit.cpp \
           kontotreeitem.cpp \
           GetOpt.cpp \
           descdata.cpp \
           smallfontdialog.cpp \
           bereitschaftsdateninfozeit.cpp \
           bereitschaftsliste.cpp \
           bereitschaftsview.cpp \
           bereitschaftsmodel.cpp \
           lock.cpp \
           colorchooser.cpp
HEADERS += abteilungsliste.h \
           lock.h \
           kontoliste.h \
           statusbar.h \
           datedialog.h \
           kontotreeitem.h \
           timecounter.h \
           eintragsliste.h \
           timeedit.h \
           kontotreeview.h \
           findkontodialog.h \
           timemainwindow.h \
           globals.h \
           toolbar.h \
           unterkontodialog.h \
           kontodateninfo.h \
           sctimehelp.h \
           unterkontoeintrag.h \
           sctimexmlsettings.h \
           unterkontoliste.h \
           preferencedialog.h \
           defaultcommentreader.h \
           utils.h \
           qdatepicker.h \
           defaulttagreader.h \
           kontodateninfozeit.h \
           GetOpt.h \
           descdata.h \
           smallfontdialog.h \
           bereitschaftsdateninfo.h \
           bereitschaftsdateninfozeit.h \
           bereitschaftsliste.h \
           bereitschaftsview.h \
           bereitschaftsmodel.h \
           colorchooser.h
RESOURCES= ../pics/sctimeImages.qrc
FORMS += datedialogbase.ui \
         preferencedialogbase.ui \
         colorchooserbase.ui
#target.path = /$(prefix)/bin
INSTALLS += target
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
   QT += sql
  # CONFIG -=debug # Ich haette gerne, dass er unter Windows _nur_ Release baut.
  CONFIG += embed_manifest_exe release
  QMAKE_CXXFLAGS += -EHsc # C++-Ausnahmen
  SOURCES += kontodateninfodatabase.cpp bereitschaftsdateninfodatabase.cpp DBConnector.cpp
  HEADERS += kontodateninfodatabase.h bereitschaftsdateninfodatabase.h DBConnector.h
  RC_FILE += sctime.rc
  LIBS += ws2_32.lib
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
mac {
  ICON = ../pics/scTime.icns
  QMAKE_INFO_PLIST = ../extra/mac/Info.plist
  #TARGET = ../bin/scTime
  QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.5.sdk
  DEFINES += MACOS
  CONFIG += x86 ppc
}
#LIBS+=-L../../gdbmacros -lgdbmacros

!win32{
  BUILDDATESTR = '"`date`"'
  SOURCES += signalhandler.cpp
  HEADERS += signalhandler.h
}
isEmpty(BUILDDATESTR){
  BUILDDATESTR = '"unknown"'
}
