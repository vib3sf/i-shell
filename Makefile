TARGET = i-shell

$(TARGET): main.c parse.c exec.c exec.c command.c stream.c
	gcc -Wall -g -o $(TARGET) main.c parse.c exec.c command.c stream.c sig_handlers.c

