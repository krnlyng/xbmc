SRCS=Album.cpp \
     Artist.cpp \
     ContextMenus.cpp \
     GUIViewStateMusic.cpp \
     MusicDatabase.cpp \
     MusicDbUrl.cpp \
     MusicInfoLoader.cpp \
     MusicThumbLoader.cpp \
     Song.cpp \
     CueInfoLoader.cpp \

LIB=music.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
