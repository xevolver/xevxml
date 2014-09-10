LD	= $(CXX)

ROSEROOT = /home/tacky/local
XEROOT = 
XAROOT = 
CXXFLAGS= -g -Wall -Wno-deprecated -I../include 
CXXFLAGS += -I../include -I/home/tacky/local/include  
#LIBS	= -L/home/tacky/local/lib -lrose -lxalan-c -lxerces-c
LIBS	 += -L/home/tacky/local/lib -lrose -lxalan-c -lxerces-c

SRCS    =  ast2xml.cpp xml2ast.cpp attrib.cpp \
	 xmlutils.cpp type.cpp preproc.cpp unparse.cpp
OBJS	= ${SRCS:.cpp=.o}
DEPS	= ${SRCS:.cpp=.d}

TARGET	= src2xml xsltexec xml2src dir2xml

.SUFFIXES: .cpp .o

all : $(TARGET)

src2xml: $(OBJS) src2xml.o
	$(LD) $(OBJS) src2xml.o -o $@ $(LIBS)

xsltexec: $(OBJS) xslt.o
	$(LD) $(OBJS) xslt.o -o $@ $(LIBS)

xml2src: $(OBJS) xml2src.o
	$(LD) $(OBJS) xml2src.o -o $@ $(LIBS)

dir2xml: $(OBJS) dir2xml.o
	$(LD) $(OBJS) dir2xml.o -o $@ $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $^

-include $(DEPS)

clean:
	-@rm -fr *~ $(OBJS) src2xml.o xslt.o xml2src.o dir2xml.o $(DEPS) $(TARGET) rose_* a.out

