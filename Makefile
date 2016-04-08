CC=g++
CFLAGS = -std=c++11 -Wall -g
LDFLAGS=

INCLUDES = -Iinclude
DEFS = 

OBJ_DIR = obj
SRC_DIR = src
INC_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:$(SRC_DIR)/%.cpp=%.o))

EXEC=SoccerPlayer

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(DEFS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp include/*.h $(OBJ_DIR)
	$(CC) $(INCLUDES) $(DEFS) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir $@

clean:
	rm -vf $(BIN) *.o *.log *.trace
	rm -rf $(OBJ_DIR)
	rm -f $(SRC_DIR)/*.orig
	rm -f $(INC_DIR)/*.orig

format:
	astyle --style=linux $(SRC_DIR)/* $(INC_DIR)/*
	rm -f $(SRC_DIR)/*.orig
	rm -f $(INC_DIR)/*.orig

#Use for Makefile debug
print:
	$(info $$SRC is [$(SRC)]) 
	$(info $$OBJ is [$(OBJ)])



.SUFFIXES:
.PHONY: all check clean debug final verbose print
