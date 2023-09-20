TARGET = i-shell

$(TARGET): main.c parse.c exec.c
	gcc -o $(TARGET) main.c parse.c exec.c
