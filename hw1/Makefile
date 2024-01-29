LEX = flex
CC = gcc
CXX = g++

LEX_SRC = src/lex.yy
LEX_OUT = lex.yy.c
LEX_OBJ = lex.yy.o
CPP_SRC = src/hw1.cpp
TARGET = hw1

all: $(TARGET)

$(LEX_OUT): $(LEX_SRC)
	$(LEX) -o$(LEX_OUT) $(LEX_SRC)

$(LEX_OBJ): $(LEX_OUT)
	$(CC) -c $(LEX_OUT) -o $(LEX_OBJ)

$(TARGET): $(LEX_OBJ) $(CPP_SRC)
	$(CXX) -std=c++11 $(LEX_OBJ) $(CPP_SRC) -o $(TARGET)

clean:
	rm -f $(LEX_OUT) $(LEX_OBJ) $(TARGET)