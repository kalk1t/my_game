# ===CONFIGURATION===

CC=gcc
CFLAGS= -Wall -Wextra -I./include -I./lib/glfw-3.4/include


SRC_DIR=src
OBJ_DIR=build
BIN_DIR=bin

TARGET=$(BIN_DIR)/my_game

SOURCES := $(wildcard $(SRC_DIR)/*.c)

OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

UNAME := $(shell uname)

ifeq ($(UNAME),Linux)
	LDFLAGS = -lglfw -lGL -ldl -lm
else
	LDFLAGS = -L./lib/glfw-3.4/lib -lglfw3 -lopengl32 -lgdi32
endif

# ===BUILD RULES===

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build successful!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(BIN_DIR)/my_game.exe
print:
	@echo $(SOURCES)

