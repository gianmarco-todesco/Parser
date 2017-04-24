CC=g++
OLD_CFLAGS=-c -ansi -O3 -pedantic -Wall -Wextra \
    -std=c++98 -pedantic-errors -Werror

CFLAGS=-c -g -Wall -fexceptions -Wnon-virtual-dtor -Wshadow -Winit-self -Wredundant-decls \
   -Wcast-align -Wundef -Wfloat-equal -Winline \
   -Wswitch-enum -Wswitch-default \
   -pedantic-errors -pedantic -std=c++98 -Wextra -Werror \
   -I../include

LDFLAGS=-lutil

SOURCES=../src/token.cpp ../src/grammar.cpp \
        ../test/main.cpp ../test/tokenizer_test.cpp 

OBJECTS=$(SOURCES:.cpp=.o)


all : test_all

test_all : $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o targetIT
