CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Iinclude -Isrc -fsanitize=address -g -O1
LDFLAGS := 
AR      := ar
ARFLAGS := rcs

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := $(OBJ_DIR)/bin
LIBNAME := libpdsn.a
TARGET  := pdsn

SRC_LIB := $(SRC_DIR)/pdsn.c $(SRC_DIR)/regex.c $(SRC_DIR)/format.c
OBJ_LIB := $(SRC_LIB:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_MAIN := $(OBJ_DIR)/main.o

.PHONY: all clean dirs

all: dirs $(LIBNAME) $(TARGET)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(LIBNAME): $(OBJ_LIB)
	$(AR) $(ARFLAGS) $(BIN_DIR)/$@ $^

$(TARGET): $(OBJ_MAIN) $(BIN_DIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: main.c include/pdsn.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(LIBNAME) $(TARGET)
