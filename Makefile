
CC=gcc
CPP=g++
LD=ld
AR=ar

BUILD_DIR=build


C_INCLUDE_PATH=/usr/include/x86_64-linux-gnu
CPLUS_INCLUDE_PATH=/usr/include/x86_64-linux-gnu


# file match pattern
# eg: "*wifi*.c" -- match the file that contains *wifi*.c in the path
# if not use this, please define as "*"
FILE_PATTERN = "*elf*.c" "*elf*.cpp"

FOR32 := true
BIN:= bin
LIB:= $(BUILD_DIR)/libelf.a

HW_LITEOSDIR:=.
ifeq ($(FOR32), true)
CFLAGS := -m32  -march=i386 -mtune=i386
else
CFLAGS := 
endif


CFLAGS += -g -w
CFLAGS += -fprofile-arcs -ftest-coverage
GCOV_FLAGS := --rc lcov_branch_coverage=1 --rc genhtml_branch_coverage=1

#compile dynamic library of liteos module
#SRC+= $(wildcard $(HW_LITEOSDIR)/stub.c)


LIB_OBJ:=$(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.c, %.o, $(SRC))))
vpath %.c $(sort $(dir $(SRC)))

INCLUDES:= \
    -I$(HW_LITEOSDIR)/include/


CPP_SRC := $(wildcard *.cpp)
CPP_OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.cpp, %.o, $(CPP_SRC))))

C_SRC := $(wildcard *.c)
C_OBJ := $(addprefix $(BUILD_DIR)/, $(notdir $(patsubst %.c, %.o, $(C_SRC))))

all:$(BUILD_DIR) lib $(BIN) 
	@echo "elf OK"

lib:$(LIB)

TEST_GCDA:=$(patsubst %.o, %.gcda, $(CPP_OBJ))
TEST_GCDA+=$(patsubst %.o, %.gcda, $(C_OBJ))
TEST_GCDA+=$(patsubst %.o, %.gcda, $(LIB_OBJ))

TEST_GCNO:=$(patsubst %.o, %.gcno, $(CPP_OBJ))
TEST_GCNO+=$(patsubst %.o, %.gcno, $(C_OBJ))
TEST_GCNO+=$(patsubst %.o, %.gcno, $(LIB_OBJ))

$(BIN):$(C_OBJ) $(CPP_OBJ)
	@$(CPP) $(CFLAGS) $(INCLUDES) -o $@ $^ -L$(BUILD_DIR)

$(CPP_OBJ):$(BUILD_DIR)/%.o:%.cpp
	@$(CPP) -c $< $(INCLUDES) $(CFLAGS) -o $@

$(C_OBJ):$(BUILD_DIR)/%.o:%.c
	@$(CC) -c $< $(INCLUDES) $(CFLAGS) -o $@

$(LIB):$(LIB_OBJ)
	@$(AR) -r $@ $^

$(LIB_OBJ):$(BUILD_DIR)/%.o:%.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
    
clean:
	rm -rf $(BUILD_DIR) bin gcov *.htm *.log

run:
	./$(BIN)

cov:
	mkdir -p gcov
	./$(BIN)
	lcov -c -o gcov/gcov.info -d . -b . --rc lcov_branch_coverage=1
	lcov -e gcov/gcov.info $(FILE_PATTERN) -o gcov/gcov.info --rc lcov_branch_coverage=1
	genhtml -o gcov/html gcov/gcov.info --rc genhtml_branch_coverage=1

memcheck:
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes --trace-children=yes  --smc-check=all ./$(BIN) 2>&1 | tee memcheck.log
