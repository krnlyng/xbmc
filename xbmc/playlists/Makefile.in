SRCS=PlayListB4S.cpp \
     PlayList.cpp \
     PlayListFactory.cpp \
     PlayListM3U.cpp \
     PlayListPLS.cpp \
     PlayListURL.cpp \
     PlayListWPL.cpp \
     PlayListXML.cpp \
     SmartPlayList.cpp \
     SmartPlaylistFileItemListModifier.cpp

LIB=playlists.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
