TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS = `pkg-config --cflags --libs glib-2.0`
QMAKE_CXXFLAGS = $$QMAKE_CFLAGS

LIBS += -lrt -levent
LIBS += -levent -lglib-2.0 -lpthread -luv
INCLUDEPATH +=/usr/include/glib-2.0/ ./
SOURCES += main.c \
    glib_queue.c

HEADERS += \
    glib_queue.h
