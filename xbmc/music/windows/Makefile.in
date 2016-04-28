SRCS=GUIWindowMusicBase.cpp \
     GUIWindowMusicNav.cpp \
     GUIWindowMusicPlaylist.cpp \
     GUIWindowMusicPlaylistEditor.cpp \
     GUIWindowVisualisation.cpp \
     MusicFileItemListModifier.cpp \

LIB=musicwindows.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
