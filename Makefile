CC := gcc
CFLAGS := -std=c99 -Werror -Wall -Wextra -Wfatal-errors -g -Wno-unused-parameter
LIBS := -lncurses

spreadmain: spreadmain.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
