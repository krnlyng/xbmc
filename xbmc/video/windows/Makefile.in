SRCS=GUIWindowFullScreen.cpp \
     GUIWindowVideoBase.cpp \
     GUIWindowVideoNav.cpp \
     GUIWindowVideoPlaylist.cpp \
     VideoFileItemListModifier.cpp \

LIB=videowindows.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
