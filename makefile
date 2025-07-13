# ===CONFIGURATION===

CC=gcc
CFLAGS= -Wall -Wextra -Linclude -lsrc

SRC_DIR=src
OBJ_DIR=build
BIN_DIR=bin

TARGET=$(BIN_DIR)/my_game

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/*.c,$(OBS_DIR)/%.o,$(SOURCES))

UNAME := $(shell uname)

ifeq ($(UNAME),Linux)
	LIBS= -lglfw -lGL -ldl -lm
else
	LIBS= -lglfw3 -lopengl32 -lgdi32
endif

# ===BUILD RULES===

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)
	@echo "Build successful!"

$(OBS_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBS_DIR) $(BIN_DIR)

