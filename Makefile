CXX=       	g++
CXXFLAGS= 	-g -gdwarf-2 -std=gnu++11 -Wall -fPIC
LDFLAGS=	-pthread -lstdc++


CFLAGS += -ggdb
CPPFLAGS = -g



# all:    test1 test2 test3 test4 test5 test6 test7

# %.o:	%.cpp uthread.h
# 	$(CXX) $(CXXFLAGS) -c -o $@ $<

# test1:	uthread.o test1.o
# 	$(CXX) $(LDFLAGS) -o $@ $^

# test2:	uthread.o test2.o
# 	$(CXX) $(LDFLAGS) -o $@ $^

# test3:	uthread.o test3.o
# 	$(CXX) $(LDFLAGS) -o $@ $^

# test4:	uthread.o test4.o
# 	$(CXX) $(LDFLAGS) -o $@ $^

# test5:	uthread.o test5.o
# 	$(CXX) $(LDFLAGS) -o $@ $^

# test6:	uthread.o test6.o
# 	$(CXX) $(LDFLAGS) -o $@ $^

# test7:	uthread.o test7.o
# 	$(CXX) $(LDFLAGS) -o $@ $^
uthread: uthread.o main.o 
uthread.o: uthread.h
main.o: uthread.h
# clean:
# 	$(RM) -f test1 test2 *.o
clean:
	$(RM) -f uthread uthread.o main.o
.PHONY: all clean

