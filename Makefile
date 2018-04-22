BIN = ./
SRC = ./src
INC = ./include
OBJ = ./obj

OBJ_FILES = $(OBJ)/util.o \
			$(OBJ)/clustering.o \
			$(OBJ)/subspace.o \
			$(OBJ)/inscy_init.o \
			$(OBJ)/inscy_algorithm.o \
			$(OBJ)/inscy_cpu.o \
			$(OBJ)/math_unsigned.o \
			$(OBJ)/main.o

CC = g++

LFLAGS = -fopenmp

CFLAGS = -c -I$(INC) --std=c++11 -fopenmp -O3

all: $(BIN)/main

$(BIN)/main: $(OBJ_FILES)
	$(CC) $(LFLAGS) -o $@ $(OBJ_FILES) -lm

$(OBJ)/main.o: $(SRC)/main.cpp $(INC)/main.h
	$(CC) $(CFLAGS) -o $(OBJ)/main.o $(SRC)/main.cpp

$(OBJ)/inscy_init.o: $(SRC)/inscy_init.cpp $(INC)/inscy.h
	$(CC) $(CFLAGS) -o $(OBJ)/inscy_init.o $(SRC)/inscy_init.cpp

$(OBJ)/inscy_algorithm.o: $(SRC)/inscy_algorithm.cpp $(INC)/inscy.h
	$(CC) $(CFLAGS) -o $(OBJ)/inscy_algorithm.o $(SRC)/inscy_algorithm.cpp

$(OBJ)/inscy_cpu.o: $(SRC)/inscy_cpu.cpp $(INC)/inscy.h
	$(CC) $(CFLAGS) -o $(OBJ)/inscy_cpu.o $(SRC)/inscy_cpu.cpp

$(OBJ)/subspace.o: $(SRC)/subspace.cpp $(INC)/subspace.h
	$(CC) $(CFLAGS) -o $(OBJ)/subspace.o $(SRC)/subspace.cpp

$(OBJ)/clustering.o: $(SRC)/clustering.cpp $(INC)/clustering.h
	$(CC) $(CFLAGS) -o $(OBJ)/clustering.o $(SRC)/clustering.cpp

$(OBJ)/util.o: $(SRC)/util.cpp $(INC)/util.h
	$(CC) $(CFLAGS) -o $(OBJ)/util.o $(SRC)/util.cpp

$(OBJ)/math_unsigned.o:
	$(CC) $(CFLAGS) -o $(OBJ)/math_unsigned.o $(INC)/math_unsigned.cpp

clean:
	rm $(OBJ)/*.o
	rm $(BIN)/main
