LDFLAGS +=  -pthread -lstdc++ 
CFLAGS += -ggdb
CPPFLAGS = -g
uthread: uthread.o main.o 
uthread.o: uthread.h
main.o: uthread.h
main2.o: uthread.h
clean:
	$(RM) -f uthread uthread.o main.o
