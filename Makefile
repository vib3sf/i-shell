TARGET = i-shell

$(TARGET): main.c parse.c exec.c
	gcc -Wall -g -o $(TARGET) main.c parse.c exec.c

