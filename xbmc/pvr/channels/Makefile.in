SRCS=PVRChannel.cpp \
     PVRChannelGroup.cpp \
     PVRChannelGroupInternal.cpp \
     PVRChannelGroups.cpp \
     PVRChannelGroupsContainer.cpp \
     PVRRadioRDSInfoTag.cpp

LIB=pvrchannels.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
