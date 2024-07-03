CFLAGS 	:= -s
CFLAGS 	+= -std=c2x
CFLAGS 	+= -no-pie
CFLAGS  += -g3
CFLAGS  += -ggdb
CFLAGS 	+= -Wall
CFLAGS 	+= -Wextra
CFLAGS 	+= -Warray-bounds
CFLAGS 	+= -Wconversion
CFLAGS 	+= -Wmissing-braces
CFLAGS 	+= -Wno-parentheses
CFLAGS 	+= -Wno-format-truncation
CFLAGS 	+= -Wpedantic
CFLAGS 	+= -Wstrict-prototypes
CFLAGS 	+= -Wwrite-strings
CFLAGS 	+= -Winline
CFLAGS 	+= -O2

NAME	:= liballoc_$(shell uname -m)-$(shell uname -s)
LIB_DIR	:= bin
SLIB  	:= $(LIB_DIR)/$(NAME).a
DLIB  	:= $(LIB_DIR)/$(NAME).so
SRC_DIR := src
SRCS 	:= $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR := obj
OBJS 	:= $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) 

all: $(SLIB) $(DLIB)

$(SLIB): $(OBJS) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^ 

$(DLIB): $(OBJS) | $(LIB_DIR)
	$(CC) $(CFLAGS) -fPIC -shared $(SRCS) -o $@

$(LIB_DIR):
	mkdir $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) -rf $(OBJ_DIR) $(LIB_DIR)

fclean:
	$(RM) $(SLIB) $(DLIB)

.PHONY: fclean clean all
.DELETE_ON_ERROR:
