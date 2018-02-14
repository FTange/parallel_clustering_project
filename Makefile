BIN = ./
SRC = ./src
INC = ./include
OBJ = ./obj

OBJ_FILES = $(OBJ)/util.o \
			$(OBJ)/clustering.o \
			$(OBJ)/main.o

CC = g++

CFLAGS = -c -I$(INC)

all: $(BIN)/main

$(BIN)/main: $(OBJ_FILES)
	$(CC) $(LFLAGS) -o $@ $(OBJ_FILES) -lm

$(OBJ)/main.o: $(SRC)/main.cpp $(INC)/main.h
	$(CC) $(CFLAGS) -o $(OBJ)/main.o $(SRC)/main.cpp

$(OBJ)/clustering.o: $(SRC)/clustering.cpp $(INC)/main.h
	$(CC) $(CFLAGS) -o $(OBJ)/clustering.o $(SRC)/clustering.cpp

$(OBJ)/util.o: $(SRC)/util.cpp $(INC)/util.h
	$(CC) $(CFLAGS) -o $(OBJ)/util.o $(SRC)/util.cpp

clean:
	rm $(OBJ)/*.o
