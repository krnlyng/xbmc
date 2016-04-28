SRCS=YUV2RGBShader.cpp VideoFilterShader.cpp ConvolutionKernels.cpp GLSLOutput.cpp

LIB=VideoShaders.a

include @abs_top_builddir@/Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(SRCS)))
