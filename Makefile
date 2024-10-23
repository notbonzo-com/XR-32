CXX        = g++
ASM        = nasm

ROOT       = $(abspath .)
SRC_DIR    = $(ROOT)/emulator
BUILD_DIR  = $(ROOT)/build
BIN_DIR    = $(ROOT)/bin
INCLUDE_DIRS = $(SRC_DIR)/include $(SRC_DIR)/include/components/io_extern
BINARY_NAME= xr32-tool

COLOR_GREEN = \033[0;32m
COLOR_YELLOW = \033[0;33m
COLOR_RESET  = \033[0m

CXXFLAGS   = -std=c++23 -O2 -Wall -Wextra -Werror -pedantic -Wshadow
INCLUDES   = $(addprefix -I, $(INCLUDE_DIRS))

SRC_FILES  = $(shell find $(SRC_DIR) -name "*.cpp")
OBJ_FILES  = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

.PHONY: all clean reset run

all: $(BIN_DIR)/$(BINARY_NAME)

$(BIN_DIR)/$(BINARY_NAME): $(OBJ_FILES) | $(BIN_DIR)
	@echo -e "$(COLOR_GREEN)Linking $@$(COLOR_RESET)"
	@$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo -e "$(COLOR_GREEN)Compiling $@$(COLOR_RESET)"
	@$(CXX) -c -o $@ $< $(CXXFLAGS) $(INCLUDES)

clean:
	@clear
	@echo -e "$(COLOR_YELLOW)Cleaning build and bin directories$(COLOR_RESET)"
	@rm -rf $(BIN_DIR) $(BUILD_DIR)

reset: clean all

run: $(BIN_DIR)/$(BINARY_NAME)
	@echo "---------"
	@$(BIN_DIR)/$(BINARY_NAME)
	@echo "---------"

$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@
