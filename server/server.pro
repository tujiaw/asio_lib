TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../asio/base/AsioClient.cpp \
    ../asio/base/AsioServer.cpp \
    ../asio/base/Buffer.cpp \
    ../asio/base/ProtoHelp.cpp \
    ../asio/base/session.cpp \
    ../asio/base/TaskManager.cpp \
    ../asio/base/threadpool.cpp \
    ../asio/base/threadpool_p.cpp \
    ../asio/base/threadpoolthread.cpp \
    ../asio/base/util.cpp \
    ../asio/proto/pb_base.pb.cc \
    ../asio/msgclient.cpp \
    ../asio/msgserver.cpp

HEADERS += \
    ../asio/base/AsioClient.h \
    ../asio/base/AsioServer.h \
    ../asio/base/Buffer.h \
    ../asio/base/desc.h \
    ../asio/base/ProtoHelp.h \
    ../asio/base/runnable.h \
    ../asio/base/session.h \
    ../asio/base/TaskManager.h \
    ../asio/base/threadpool.h \
    ../asio/base/threadpool_p.h \
    ../asio/base/threadpoolthread.h \
    ../asio/base/util.h \
    ../asio/proto/pb_base.pb.h \
    ../asio/msgclient.h \
    ../asio/msgserver.h

INCLUDEPATH += $$PWD/../

# boost
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_chrono-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_chrono-vc120-mt-gd-1_65_1
else:unix:!macx: LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_chrono-vc120-mt-gd-1_65_1

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_date_time-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_date_time-vc120-mt-gd-1_65_1
else:unix:!macx: LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_date_time-vc120-mt-gd-1_65_1

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_regex-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_regex-vc120-mt-gd-1_65_1
else:unix:!macx: LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_regex-vc120-mt-gd-1_65_1

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_system-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_system-vc120-mt-gd-1_65_1
else:unix:!macx: LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_system-vc120-mt-gd-1_65_1

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_thread-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_thread-vc120-mt-gd-1_65_1
else:unix:!macx: LIBS += -L$$PWD/../asio/third/boost/lib/ -llibboost_thread-vc120-mt-gd-1_65_1

INCLUDEPATH += $$PWD/../asio/third/boost/include
DEPENDPATH += $$PWD/../asio/third/boost/lib

# glog
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../asio/third/glog/lib/ -lglog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../asio/third/glog/lib/ -lglogd
else:unix:!macx: LIBS += -L$$PWD/../asio/third/glog/lib/ -lglog

INCLUDEPATH += $$PWD/../asio/third/glog/include
DEPENDPATH += $$PWD/../asio/third/glog/lib

# protobuf
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../asio/third/protobuf/lib/ -llibprotobuf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../asio/third/protobuf/lib/ -llibprotobufd
else:unix:!macx: LIBS += -L$$PWD/../asio/third/protobuf/lib/ -llibprotobuf

INCLUDEPATH += $$PWD/../asio/third/protobuf/include
DEPENDPATH += $$PWD/../asio/third/protobuf/lib

# zlib
unix:!macx|win32: LIBS += -L$$PWD/../asio/third/zlib/lib/ -lzlibstatic

INCLUDEPATH += $$PWD/../asio/third/zlib/include
DEPENDPATH += $$PWD/../asio/third/zlib/lib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../asio/third/zlib/lib/zlibstatic.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/../asio/third/zlib/lib/libzlibstatic.a

DISTFILES += \
    ../asio/proto/pb_base.proto
