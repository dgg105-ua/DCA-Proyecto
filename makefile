.PHONY: all clean install uninstall

CXX = ccache g++
CXXFLAGS = -I src/ -I vendor/include/ -L vendor/lib -lraylib -lGL -lm -lpthread -lrt -lX11

SRC_DIRS = src/core src/states
OBJ_DIR = obj
TARGET = ProjectPrime8

PREFIX ?= /usr
DESTDIR ?= 

SRCS = $(wildcard $(addsuffix /*.cpp, $(SRC_DIRS)))
OBJS = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/core
	mkdir -p $(OBJ_DIR)/states

install: all
	install -D -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall: clean
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)