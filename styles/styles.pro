QT += widgets dtkwidget

HEADERS       = norwegianwoodstyle.h \
                widgetgallery.h
SOURCES       = main.cpp \
                norwegianwoodstyle.cpp \
                widgetgallery.cpp
RESOURCES     = styles.qrc

DESTDIR = $$_PRO_FILE_PWD_/../bin/

CONFIG += c++11

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/styles
# INSTALLS += target
