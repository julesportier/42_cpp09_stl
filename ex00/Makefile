###########
# ALIASES #
###########
MAKE := make
CXX := c++

#########
# FLAGS #
#########
STD_FLAGS :=		-std=c++98
WARNINGS :=			-Wall -Werror -Wextra
EXTRA_WARNINGS :=	-Wundef -Wshadow -Wpointer-arith -Wsign-conversion -Wconversion
CXXFLAGS :=			$(STD_FLAGS) $(WARNINGS)
CXXFLAGS_DB :=		$(STD_FLAGS) $(WARNINGS) $(EXTRA_WARNINGS) -g3
CXXFLAGS_GDB :=		$(STD_FLAGS) $(WARNINGS) $(EXTRA_WARNINGS) -ggdb
CXXFLAGS_NE :=		$(STD_FLAGS) -Wall -Wextra -g3
CXXFLAGS_AS :=		$(STD_FLAGS) $(WARNINGS) $(EXTRA_WARNINGS) -g3 \
					-fsanitize=address
CXXFLAGS_US :=		$(STD_FLAGS) $(WARNINGS) $(EXTRA_WARNINGS) -g3 \
					-fsanitize=undefined
DEP_FLAGS := 		-MMD

#########
# FILES #
#########
NAME := btc

SRC_DIR := src
vpath %.cpp $(SRC_DIR)
SRC := main.cpp \
	   BitcoinExchange.cpp \
	   Date.cpp \
	   DataBase.cpp \
	   utils.cpp

BUILD_DIR := build
OBJ := $(SRC:%.cpp=$(BUILD_DIR)/%.o)

DEP_FILES := $(OBJ:.o=.d)

###########
# TARGETS #
###########
.PHONY: all clean fclean re dbg gdb noerr asan usan

all: $(BUILD_DIR) $(NAME)

dbg: CXXFLAGS = $(CXXFLAGS_DB)
dbg: CXXFLAGS += -DDBG
dbg: all

gdb: CXXFLAGS = $(CXXFLAGS_GDB)
gdb: CXX = g++
gdb: all

noerr: CXXFLAGS = $(CXXFLAGS_NE)
noerr: all

asan: CXXFLAGS = $(CXXFLAGS_AS)
asan: CXX = clang++
asan: all
usan: CXXFLAGS = $(CXXFLAGS_US)
usan: CXX = clang++
usan: all

# Only print a message if actually removing files/folders
rm_wrapper = rm -r $(1) 2>/dev/null && echo $(2) || true
clean:
	@$(call rm_wrapper,$(BUILD_DIR),"remove $(BUILD_DIR)/")
fclean: clean
	@$(call rm_wrapper,$(NAME),"remove $(NAME)")

re: fclean all

#########
# RULES #
#########
-include $(DEP_FILES)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) $(DEP_FLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir $@
