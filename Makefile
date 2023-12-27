#------------------------------------------------------------------------------
#
#	LazyCat Design
#
#	Ethan Tsai
#	yuqingcai@gmail.com
#	2023-12-18
#
#------------------------------------------------------------------------------
CC = gcc
CXX = g++
AS = as
LD = ld
AR = ar
OBJCOPY	= objcopy
OBJDUMP	= objdump
INCLUDE_DIR = -I/usr/local/include

LIBS= -lz

C_FLAGS = $(INCLUDE_DIR) -Os
CPP_FLAGS = $(INCLUDE_DIR) -Os
CXX_FLAGS = $(INCLUDE_DIR) -std=c++20 -Wall -Wextra -Os

%.o: %.c
	$(CC) $(C_FLAGS) -c -g $< -o $@
%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -g $< -o $@
%.o: %.cc
	$(CXX) $(CXX_FLAGS) -c -g $< -o $@
%.d: %.cpp
	$(CXX) $(CXX_FLAGS) -MM $< -MT $*.o > $@
%.d: %.cc
	$(CXX) $(CXX_FLAGS) -MM $< -MT $*.o > $@
%.d: %.c
	$(CC) $(C_FLAGS) -MM $< -MT $*.o > $@

dr: main.o MnistLoader.o Image.o Helper.o Network.o Network2.o
	$(CXX) $^ -o $@ $(LIBS)

clean:
	rm -f dr
	rm -f *.o

