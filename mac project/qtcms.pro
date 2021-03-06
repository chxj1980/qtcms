TEMPLATE = subdirs

TOPDIR = ..

SUBDIRS += \
    ../src/QTTest \
    ../src/libpcom \
    ../src/activities/previewactivity \
    ../src/activities/Settingsactivity \
    ../src/activities/PlaybackActivity \
    ../src/activities/BackupActivity \
    ../src/functionlibs/AudioDecoder \
#    ../src/functionlibs/AudioDevice \ need to be another component to run under MAC OS
    ../src/functionlibs/AudioPlayer \
    ../src/functionlibs/BubbleProtocol \
    ../src/functionlibs/BubbleProtocolEx \
#   ../src/functionlibs/DDrawRenderObject \
#   ../src/functionlibs/DdrawRender \ must use another component to instead it
    ../src/functionlibs/DeviceClient \
    ../src/functionlibs/UnixCommonTools \
    ../src/functionlibs/DvrSearch \
    ../src/functionlibs/HiChipSearch

