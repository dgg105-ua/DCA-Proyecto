.PHONY: all clean install

CXX = ccache g++
CXXFLAGS = -I src/ -I vendor/include/ -L vendor/lib -lraylib -lGL -lm -lpthread -lrt -lX11

SRC_DIRS = src/core src/states
OBJ_DIR = obj
DEST_DIR = /usr/local/bin
TARGET = game

SRCS = $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(DEST_BIN):
	mkdir -p $(DEST_DIR)/usr/bin

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

install: $(TARGET) | $(DEST_BIN)
	cp $(TARGET) $(DEST_DIR)/usr/bin/
