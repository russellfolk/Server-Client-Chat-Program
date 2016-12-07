#! /usr/bin/env make
# Makefile for Client-Server Chat Program

# Compiler to use with settings
CC      = gcc -std=c99 -pipe

# Compiler Flags
FLAGS   = -pedantic -ggdb -I$(INC_DIR)
CFLAGS  = -O2 $(FLAGS)

# Linker Settings
LINKER  = $(CC)
LDLIBS  = -I$(INC_DIR) -lpthread
LDFLAGS = -Wall $(LDLIBS)

# Directories to use
INC_DIR = include
OBJ_DIR = obj
OUT_DIR = bin
SRC_DIR = src

SOURCES := $(wildcard $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(INC_DIR)/*.h)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

rm       = rm -f

all: server client

server: $(OBJ_DIR)/server.o
	$(LINKER) -o $(OUT_DIR)/server $(OBJ_DIR)/server.o $(LDFLAGS)

$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c $(INC_DIR)/server.h $(INC_DIR)/bool.h $(INC_DIR)/user.h $(INC_DIR)/connection_t.h $(INC_DIR)/chat_message.h
	$(CC) -o $(OBJ_DIR)/server.o $(CFLAGS) -c $(SRC_DIR)/server.c

client: $(SRC_DIR)/client.c $(SRC_DIR)/clear.c $(INC_DIR)/bool.h $(INC_DIR)/chat_message.h $(INC_DIR)/clear.h
	$(CC) $(CFLAGS) -Wall -o bin/client src/client.c src/clear.c -lpthread

.PHONEY: clean

clean:
	$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(OUT_DIR)/*
	@echo "Executable removed!"
