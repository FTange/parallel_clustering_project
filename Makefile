BIN = ./
SRC = ./src
INC = ./include
OBJ = ./obj

OBJ_FILES = $(OBJ)/util.o \
			$(OBJ)/clustering.o \
			$(OBJ)/subspace.o \
			$(OBJ)/main.o

CC = g++

LFLAGS = -fopenmp

CFLAGS = -c -I$(INC) --std=c++11 -fopenmp

all: $(BIN)/main

$(BIN)/main: $(OBJ_FILES)
	$(CC) $(LFLAGS) -o $@ $(OBJ_FILES) -lm

$(OBJ)/main.o: $(SRC)/main.cpp $(INC)/main.h
	$(CC) $(CFLAGS) -o $(OBJ)/main.o $(SRC)/main.cpp

$(OBJ)/subspace.o: $(SRC)/subspace.cpp $(INC)/subspace.h
	$(CC) $(CFLAGS) -o $(OBJ)/subspace.o $(SRC)/subspace.cpp

$(OBJ)/clustering.o: $(SRC)/clustering.cpp $(INC)/clustering.h
	$(CC) $(CFLAGS) -o $(OBJ)/clustering.o $(SRC)/clustering.cpp

$(OBJ)/util.o: $(SRC)/util.cpp $(INC)/util.h
	$(CC) $(CFLAGS) -o $(OBJ)/util.o $(SRC)/util.cpp

clean:
	rm $(OBJ)/*.o
	rm $(BIN)/main
