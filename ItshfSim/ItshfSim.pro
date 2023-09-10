QT += core gui

# 添加组件
QT += charts
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    env/model.cpp \
    env/sqlite3.c \
    env/dbcreator.cpp \
    env/randmng.cpp \
    env/wenv.cpp \
    link/linkcfg.cpp \
    link/linksim.cpp \
    auto/autocfg.cpp \
    auto/autosim.cpp \
    win/wchart.cpp \
    win/wlabel.cpp \
    mainwin.cpp \
    main.cpp

HEADERS += \
    macro.h \
    type.h \
    env/model.h \
    env/sqlite3.h \
    env/dbcreator.h \
    env/randmng.h \
    env/wenv.h \
    auto/autocfg.h \
    auto/autosim.h \
    link/linkcfg.h \
    link/linksim.h \
    win/wchart.h \
    win/wlabel.h \
    mainwin.h \
    mainwin.h

FORMS += \
    env/dbcreator.ui \
    env/model.ui \
    link/linkcfg.ui \
    auto/autocfg.ui \
    mainwin.ui

# 界面图标
RC_ICONS += icon/app.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

# 控制优化等级
QMAKE_CXXFLAGS_DEBUG -= -O
QMAKE_CXXFLAGS_DEBUG -= -O1
QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_DEBUG -= -O3
QMAKE_CXXFLAGS_DEBUG += -O0
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE += -O2
