.PHONY: all clean

CXX = g++
CXXFLAGS = -I src/ -I vendor/include/ -L vendor/lib -lraylib -lGL -lm -lpthread -lrt -lX11

SRC_DIRS = src/core src/states
OBJ_DIR = obj
TARGET = game

SRCS = $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
