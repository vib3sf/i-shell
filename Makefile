TARGET = i-shell

CC = clang
CFLAGS = -Wall -ansi -pedantic -g

SRCPREFIX = ./src/
BINPREFIX = ./bin/

SRC = $(filter-out $(SRCPREFIX)main.c, $(wildcard $(SRCPREFIX)*.c))
OBJ = $(patsubst $(SRCPREFIX)%.c, $(BINPREFIX)%.o, $(SRC))

$(BINPREFIX)%.o: $(SRCPREFIX)%.c $(SRCPREFIX)%.h
	mkdir -p bin/
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(SRCPREFIX)main.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: $(SRC)
	$(CC) -MM $^ > $@

clean:
	rm -rf $(TARGET) $(BINPREFIX)

