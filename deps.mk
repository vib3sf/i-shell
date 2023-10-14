command.o: src/command.c src/command.h src/exec.h src/stream.h
exec.o: src/exec.c src/exec.h src/command.h src/stream.h src/sig.h
main.o: src/main.c src/command.h src/parse.h src/exec.h src/sig.h
parse.o: src/parse.c src/parse.h
sig.o: src/sig.c src/sig.h
stream.o: src/stream.c src/stream.h
