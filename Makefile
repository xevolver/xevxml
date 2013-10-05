CXX	= g++
LD	= $(CXX)

ROSEROOT	= /home/tacky/local

INCLUDE	= -I$(ROSEROOT)/include
CXXFLAGS= -g -Wall -Wno-deprecated $(INCLUDE) 

ROSELIB	= -L$(ROSEROOT)/lib -lrose -lxalan-c -lxerces-c
LIBS	= $(ROSELIB) $(JVMLIB)
SRCS    =  ast2xml.cpp xml2ast.cpp attrib.cpp \
	 xmlutils.cpp type.cpp preproc.cpp
OBJS	= ${SRCS:.cpp=.o}
DEPS	= ${SRCS:.cpp=.d}

TARGET	= src2xml xsltrans xml2src

.SUFFIXES: .cpp .o

all : $(TARGET)

src2xml: $(OBJS) src2xml.o
	$(LD) $(OBJS) src2xml.o -o $@ $(LIBS)

xsltrans: $(OBJS) xslt.o
	$(LD) $(OBJS) xslt.o -o $@ $(LIBS)

xml2src: $(OBJS) xml2src.o
	$(LD) $(OBJS) xml2src.o -o $@ $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $^

-include $(DEPS)

clean:
	-rm -fr *~ $(OBJS) src2xml.o xslt.o xml2src.o $(DEPS) $(TARGET) rose_* a.out

