LDFLAGS +=  -pthread -lstdc++ 
CFLAGS += -ggdb
uthread: uthread.o main2.o 
uthread.o: uthread.h
main.o: uthread.h
main2.o: uthread.h
clean:
	$(RM) -f uthread uthread.o main.o
