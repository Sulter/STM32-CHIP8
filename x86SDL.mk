SOURCES_DIR = src/
BUILD_DIR = build/
BIN_DIR = bin/
INC_DIR = include/
SOURCES = $(notdir $(wildcard $(SOURCES_DIR)*.cpp))
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS:= $(addprefix $(BUILD_DIR), $(OBJECTS))
DEPS = $(SOURCES:.cpp=.d)
DEPS := $(addprefix $(BUILD_DIR), $(DEPS))
EXE = main
EXE := $(addprefix $(BIN_DIR), $(EXE))
CXX = g++
CXXFLAGS = -Og -std=c++11 -Wall -Wextra -I$(INC_DIR)
LDD = -lSDL2

.PHONY: all clean help
all: $(EXE)

$(EXE): $(OBJECTS) $(DEPS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDD)

$(BUILD_DIR)%.o: $(SOURCES_DIR)%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)%.d: $(SOURCES_DIR)%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) -MT $(@:.d=.o) -MM $(CXXFLAGS) $^ > $@

clean:
	@rm -rf build
	@rm -rf bin

run: all
	./$(EXE)

help:
	@echo "You can:\n \
		make clean\n \
		make help"
