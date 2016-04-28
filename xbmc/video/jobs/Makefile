SRCS=VideoLibraryCleaningJob.cpp \
     VideoLibraryJob.cpp \
     VideoLibraryMarkWatchedJob.cpp \
     VideoLibraryProgressJob.cpp \
     VideoLibraryRefreshingJob.cpp \
     VideoLibraryScanningJob.cpp \
     
LIB=video-jobs.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
