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
LIBDIR 	:= bin
SLIB  	:= $(LIBDIR)/$(NAME).a
DLIB  	:= $(LIBDIR)/$(NAME).so
SRCS 	:= $(wildcard src/*.c)
OBJS 	:= $(patsubst src/%.c, obj/%.o, $(SRCS)) 

all: $(SLIB) $(DLIB)

$(SLIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^ 

$(DLIB): $(OBJS)
	$(CC) $(CFLAGS) -fPIC -shared $(SRCS) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -rf $(OBJS) 

fclean:
	$(RM) $(SLIB) $(DLIB)

.PHONY: fclean clean all
.DELETE_ON_ERROR:
