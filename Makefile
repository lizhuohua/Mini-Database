CC=g++
LEX=flex
YACC=bison
CFLAGS=-Iinclude -Wno-write-strings
YFLAGS=-d
LFLAGS=

all: bin/sql

bin/sql: src/main.cpp src/lexer.cpp src/parser.cpp include/parser.h include/node.h
	@mkdir -p bin
	$(CC) -std=c++11 $(CFLAGS) -o $@ src/main.cpp src/lexer.cpp src/parser.cpp

src/parser.cpp include/parser.h: config/parser.y
	@mkdir -p src
	@mkdir -p include
	$(YACC) --defines=include/parser.h -o src/parser.cpp $^

src/lexer.cpp: config/lexer.lex
	@mkdir -p src
	$(LEX) $(LFLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -f bin/*.o bin/sql src/parser.cpp src/lexer.cpp include/parser.h
