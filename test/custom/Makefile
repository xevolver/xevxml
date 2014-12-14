ROSEDIR=/usr/local/rose-current/include
TARGET=defuse
all:
	g++ -I$(ROSEDIR)/include -I../../include defuse.cpp -o $(TARGET) -L../../src/ -lxevxml -L$(ROSEDIR)/lib -lrose -lxalan-c -lxerces-c
clean:
	-@rm -rf a.out $(TARGET)
