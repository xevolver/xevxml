include ../../build/common.mk
CXXFLAGS= -g -Wall -Wno-deprecated -I../include
CXXFLAGS += $(ROSE_CFLAGS) $(XALANC_CFLAGS) $(XERCESC_CFLAGS)
LIBS  += -L. -lxevxml $(ROSE_LIBS) $(XALANC_LIBS) $(XERCESC_LIBS)
TARGET=defuse
all:
	g++ $(CXXFLAGS) -I../../include defuse.cpp -o $(TARGET) -L../../src/ -lxevxml $(LIBS)
	./$(TARGET) input_defuse.c
clean:
	-@rm -rf a.out $(TARGET)
