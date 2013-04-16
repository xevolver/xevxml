CXX	= g++
LD	= $(CXX)

ROSEROOT	= /home/tacky/local

INCLUDE	= -I$(ROSEROOT)/include
CXXFLAGS= -g -Wall -Wno-deprecated -MD $(INCLUDE) 

ROSELIB	= -L$(ROSEROOT)/lib -lrose -lxalan-c -lxerces-c
LIBS	= $(ROSELIB) $(JVMLIB)
SRCS    =  main.cpp ast2xml.cpp xml2ast.cpp attrib.cpp \
	 xmlutils.cpp type.cpp preproc.cpp xslt.cpp
OBJS	= ${SRCS:.cpp=.o}
DEPS	= ${SRCS:.cpp=.d}

TARGET	= xevxml

.SUFFIXES: .cpp .o

all : $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(OBJS) -o $@ $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $^

-include $(DEPS)

clean:
	-rm -fr *~ $(OBJS) $(DEPS) $(TARGET) rose_* a.out

