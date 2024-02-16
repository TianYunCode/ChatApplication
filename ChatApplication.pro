QT       += core gui
#以下是项目需要用到的模块
#该项目 Qt 版本为 Qt 5.14.0   工具->选项->文本编辑器->行为->文件编码 {<UTF-8 BOM>设置为 <如果编码是UTF-8则添加>} 最后保存退出
#该项目编译器版本为 MSVC2017 64bit（需要安装VS2017)
QT       += network
QT       += sql
QT       += webchannel
QT       += webengine
QT       += webenginewidgets
QT       += xml


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basicwindow.cpp \
    commonutils.cpp \
    contactitem.cpp \
    custommenu.cpp \
    emotionlabelitem.cpp \
    emotionwindow.cpp \
    main.cpp \
    ccmainwindow.cpp \
    msgwebview.cpp \
    notifymanager.cpp \
    qclicklabel.cpp \
    qmsgtextedit.cpp \
    receivefile.cpp \
    rootcontatitem.cpp \
    sendfile.cpp \
    skinwindow.cpp \
    systray.cpp \
    talkwindow.cpp \
    talkwindowitem.cpp \
    talkwindowshell.cpp \
    titlebar.cpp \
    userlogin.cpp \
    windowmanager.cpp

HEADERS += \
    basicwindow.h \
    ccmainwindow.h \
    commonutils.h \
    contactitem.h \
    custommenu.h \
    emotionlabelitem.h \
    emotionwindow.h \
    msgwebview.h \
    notifymanager.h \
    qclicklabel.h \
    qmsgtextedit.h \
    receivefile.h \
    rootcontatitem.h \
    sendfile.h \
    skinwindow.h \
    systray.h \
    talkwindow.h \
    talkwindowitem.h \
    talkwindowshell.h \
    titlebar.h \
    userlogin.h \
    windowmanager.h

FORMS += \
    ccmainwindow.ui \
    contactitem.ui \
    emotionwindow.ui \
    receivefile.ui \
    sendfile.ui \
    skinwindow.ui \
    talkwindow.ui \
    talkwindowitem.ui \
    talkwindowshell.ui \
    userlogin.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
