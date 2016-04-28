SRCS=MusicAlbumInfo.cpp \
     MusicArtistInfo.cpp \
     MusicInfoScanner.cpp \
     MusicInfoScraper.cpp \

LIB=musicscanner.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
