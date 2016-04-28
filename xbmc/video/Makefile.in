SRCS=Bookmark.cpp \
     ContextMenus.cpp \
     GUIViewStateVideo.cpp \
     PlayerController.cpp \
     Teletext.cpp \
     VideoDatabase.cpp \
     VideoDbUrl.cpp \
     VideoInfoDownloader.cpp \
     VideoInfoScanner.cpp \
     VideoInfoTag.cpp \
     VideoLibraryQueue.cpp \
     VideoReferenceClock.cpp \
     VideoThumbLoader.cpp \
     
LIB=video.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
