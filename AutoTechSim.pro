# 添加组件
QT += core gui
QT += charts
QT += network

# 添加boost库
INCLUDEPATH += F:\\setups\\boost_1_83_0

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    type.cpp \
    sql/util.cpp \
    sql/kmean.cpp \
    sql/simsql.cpp \
    sql/sqlite3.c \
    env/dbdlg.cpp \
    env/modeldlg.cpp \
    env/randmng.cpp \
    env/wenv.cpp \
    link/linkdlg.cpp \
    link/linksim.cpp \
    auto/basealg.cpp \
    auto/bisectalg.cpp \
    auto/itshfalg.cpp \
    auto/montealg.cpp \
    win/wchart.cpp \
    win/wlabel.cpp \
    colorpal.cpp \
    mainwin.cpp \
    main.cpp

HEADERS += \
    macro.h \
    type.h \
    sql/util.h \
    sql/kmean.h \
    sql/simsql.h \
    sql/sqlite3.h \
    env/dbdlg.h \
    env/modeldlg.h \
    env/randmng.h \
    env/wenv.h \
    auto/basealg.h \
    auto/bisectalg.h \
    auto/itshfalg.h \
    auto/montealg.h \
    auto/autosim.h \
    link/linkdlg.h \
    link/linksim.h \
    win/wchart.h \
    win/wlabel.h \
    colorpal.h \
    mainwin.h \
    mainwin.h

FORMS += \
    env/dbdlg.ui \
    env/modeldlg.ui \
    link/linkdlg.ui \
    colorpal.ui \
    mainwin.ui

TRANSLATIONS += \
    AutoTechSim_en_150.ts
CONFIG += lrelease
CONFIG += embed_translations

RESOURCES += \
    res.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 界面图标
RC_ICONS += icon/app.ico

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
