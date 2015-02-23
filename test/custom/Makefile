include ../../build/common.mk
CXXFLAGS= -g -Wall -Wno-deprecated -I../include
CXXFLAGS += $(ROSE_CFLAGS) $(XALANC_CFLAGS) $(XERCESC_CFLAGS)
LIBS  += -L. -lxevxml $(ROSE_LIBS) $(XALANC_LIBS) $(XERCESC_LIBS)
TARGET=defuse
all:
	#g++ $(CXXFLAGS) -I../../include defuse.cpp -o $(TARGET) -L../../src/ -lxevxml $(LIBS)
	g++ $(CXXFLAGS) -I../../include -I/home/tacky/src/rose/mybuild -c outline2.cpp -o outline2.o
	g++ $(CXXFLAGS) -I../../include -I/home/tacky/src/rose/mybuild -c autotune.cpp -o autotune.o
	g++ $(CXXFLAGS) -I../../include -I/home/tacky/src/rose/mybuild outline2.o autotune.o -o outliner -L../../src/ -lxevxml $(LIBS)
	#	./$(TARGET) input_defuse.c
clean:
	-@rm -rf a.out $(TARGET)
