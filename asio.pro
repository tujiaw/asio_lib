TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt



SOURCES += main.cpp \
    base/AsioClient.cpp \
    base/AsioServer.cpp \
    base/Buffer.cpp \
    base/ProtoHelp.cpp \
    base/session.cpp \
    base/TaskManager.cpp \
    base/threadpool.cpp \
    base/threadpool_p.cpp \
    base/threadpoolthread.cpp \
    base/util.cpp \
    proto/pb_base.pb.cc \
    msgclient.cpp \
    msgserver.cpp

DISTFILES += \
    proto/protoc.exe \
    proto/convert.bat \
    proto/pb_base.proto \
    .gitignore

HEADERS += \
    base/AsioClient.h \
    base/AsioServer.h \
    base/Buffer.h \
    base/desc.h \
    base/ProtoHelp.h \
    base/runnable.h \
    base/session.h \
    base/TaskManager.h \
    base/threadpool.h \
    base/threadpool_p.h \
    base/threadpoolthread.h \
    base/util.h \
    proto/pb_base.pb.h \
    msgclient.h \
    msgserver.h

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/third/boost/include/
INCLUDEPATH += $$PWD/third/glog/include/
INCLUDEPATH += $$PWD/third/protobuf/include/
INCLUDEPATH += $$PWD/third/zlib/include/



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/protobuf/lib/ -llibprotobuf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/protobuf/lib/ -llibprotobufd
else:unix: LIBS += -L$$PWD/third/protobuf/lib/ -llibprotobuf

INCLUDEPATH += $$PWD/third/protobuf/lib
DEPENDPATH += $$PWD/third/protobuf/lib


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/glog/lib/ -lglog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/glog/lib/ -lglogd
else:unix: LIBS += -L$$PWD/third/glog/lib/ -lglog

INCLUDEPATH += $$PWD/third/glog/lib
DEPENDPATH += $$PWD/third/glog/lib




win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_chrono-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_chrono-vc120-mt-gd-1_65_1
else:unix: LIBS += -L$$PWD/third/boost/lib/ -llibboost_chrono-vc120-mt-gd-1_65_1

INCLUDEPATH += $$PWD/third/boost/lib
DEPENDPATH += $$PWD/third/boost/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_date_time-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_date_time-vc120-mt-gd-1_65_1
else:unix: LIBS += -L$$PWD/third/boost/lib/ -llibboost_date_time-vc120-mt-gd-1_65_1

INCLUDEPATH += $$PWD/third/boost/lib
DEPENDPATH += $$PWD/third/boost/lib


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_regex-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_regex-vc120-mt-gd-1_65_1
else:unix: LIBS += -L$$PWD/third/boost/lib/ -llibboost_regex-vc120-mt-gd-1_65_1

INCLUDEPATH += $$PWD/third/boost/lib
DEPENDPATH += $$PWD/third/boost/lib


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_system-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_system-vc120-mt-gd-1_65_1
else:unix: LIBS += -L$$PWD/third/boost/lib/ -llibboost_system-vc120-mt-gd-1_65_1

INCLUDEPATH += $$PWD/third/boost/lib
DEPENDPATH += $$PWD/third/boost/lib



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_thread-vc120-mt-gd-1_65_1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/boost/lib/ -llibboost_thread-vc120-mt-gd-1_65_1
else:unix: LIBS += -L$$PWD/third/boost/lib/ -llibboost_thread-vc120-mt-gd-1_65_1

INCLUDEPATH += $$PWD/third/boost/lib
DEPENDPATH += $$PWD/third/boost/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/third/zlib/lib/ -lzlibstatic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/third/zlib/lib/ -lzlibstatic
else:unix:!macx: LIBS += -L$$PWD/third/zlib/lib/ -lzlibstatic

INCLUDEPATH += $$PWD/third/zlib/lib
DEPENDPATH += $$PWD/third/zlib/lib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/third/zlib/lib/libzlibstatic.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/third/zlib/lib/libzlibstaticd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/third/zlib/lib/zlibstatic.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/third/zlib/lib/zlibstatic.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/third/zlib/lib/libzlibstatic.a
