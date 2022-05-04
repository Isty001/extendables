CC ?= gcc
DEPS = -l:liblua.a -ldl -lm

ifdef EXT_DEBUG
	CFLAGS += -g -DEXT_DEBUG=1 -DDEBUG=1 # Compatibility with external libraries
else
	CFLAGS += -O1 -Os -DEXT_DEBUG=0 -DDEBUG=0 # Compatibility with external libraries
endif

CFLAGS += -std=c99 -Wall -Wextra -Werror -ftrapv -Wshadow -Wundef -Wcast-align -Wunreachable-code -fstack-protector -D_FORTIFY_SOURCE=2
CFLAGS += -I .

TEST_CFLAGS = $(CFLAGS)

SRC = $(shell find src -name "*.c" -not -path src/bin/*) $(shell find deps -name "*.c")
TEST_SRC = $(shell find test -name "*.c") $(SRC)

TARGET = extendables
TEST_TARGET = $(TARGET)-test

VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0

D_TARGET = $(TARGET).so
D_SONAME = lib$(TARGET).so.$(VERSION_MAJOR)

S_TARGET = $(TARGET).a

OS := $(shell uname)

ifeq ($(OS),Darwin)
	D_SONAME_ARG = -install_name,$(D_SONAME)
else
	D_SONAME_ARG = -soname,$(D_SONAME)
endif

INCLUDE_DIR = /usr/local/include/$(TARGET)
LIB_DIR = /usr/local/lib

DIR_BUILD=./build

OBJECTS = $(patsubst %.c, $(DIR_BUILD)/%.o, $(SRC))
TEST_OBJECTS = $(patsubst %.c, $(DIR_BUILD)/%.o, $(TEST_SRC))


default: $(D_TARGET) $(S_TARGET)

-include $(shell find $(DIR_BUILD) -name "*.d")


$(DIR_BUILD)/%.o: %.c
	mkdir -p $(shell dirname $@)
	$(CC) -MMD $(CFLAGS) -c -fpic $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)


$(D_TARGET): $(OBJECTS)
	$(CC) -shared -fpic -Wl,$(D_SONAME_ARG) -o $(D_TARGET) $(OBJECTS) $(DEPS)

$(S_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) $(CFLAGS) -o $@

clean:
	rm -rf $(DIR_BUILD)/*
	rm -f $(TEST_TARGET)

install: $(D_TARGET)
	sudo mv $(D_TARGET) $(LIB_DIR)/lib$(D_TARGET)
	sudo mv $(S_TARGET) $(LIB_DIR)/lib$(S_TARGET)
	sudo mkdir -p $(INCLUDE_DIR)
	sudo cp include/*.h $(INCLUDE_DIR)

compile-test:
	$(CC) $(TEST_CFLAGS) $(TEST_SRC) -o $(TEST_TARGET) $(DEPS)

test: clean compile-test
	./$(TEST_TARGET)

valgrind-test: clean compile-test
	valgrind --track-origins=yes --leak-check=full --show-reachable=yes --error-exitcode=1 ./$(TEST_TARGET)

format:
	clang-format -style=file -i $(SRC) src/*.h include/extendables.h

