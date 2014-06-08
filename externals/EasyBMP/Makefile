CXX = g++-4.8
CXXFLAGS = -O2 -g -Wall -std=c++0x
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
TARGET_DIR = lib

CXXFLAGS += -I $(INCLUDE_DIR)

# Macro helper
src_to_obj = $(addsuffix .o, $(basename $(subst $(SRC_DIR), $(OBJ_DIR), $(1))))

CXXFILES := $(wildcard $(SRC_DIR)/*.cpp)
HXXFILES := $(wildcard $(INCLUDE_DIR)/*.h)
OBJFILES := $(call src_to_obj, $(CXXFILES))

TARGET = $(TARGET_DIR)/libeasybmp.a
.DEFAULT_GOAL := $(TARGET)

# Supress makefile rescanning
Makefile: ;

# Compile static library
$(TARGET): $(OBJFILES)
	ar rcs $@ $^

# in case of multiple files in CXXFILES that may not fire.
# FIXME: replace single dependency file to one-by-one for each source.
deps.make: $(CXXFILES) $(HXXFILES)
	$(CXX) $(CXXFLAGS) -MM -MT $(OBJFILES) -MT $@ -MF $@ $(CXXFILES)

.PHONY: clean
clean:
	rm -f $(OBJFILES) $(TARGET) deps.make

# Suppress some "Unused result" warnings by doind redirection 2> /dev/null
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< 2>/dev/null

ifneq ($(MAKECMDGOALS), clean)
-include deps.make
endif
