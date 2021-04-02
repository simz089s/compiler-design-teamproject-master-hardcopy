ifeq ($(TARGET),clang)
	CC = clang
	CFLAGS = -Wall -Wno-unused -pedantic -std=gnu11 -g -glldb
else
	CC = gcc
	CFLAGS = -Wall -Wno-unused -pedantic -std=gnu11 -g -ggdb
endif

CLIBS = -lfl

main: parser.tab.o lex.yy.o main.o tree.o pretty_printer.o symbol_table.o type_checker.o codegen.o
	$(CC) $(CFLAGS) $^ -o golitec $(CLIBS)

parser.tab.c parser.tab.h: parser.y
	bison --yacc --defines=parser.tab.c --output=parser.tab.h parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
	rm *.o golitec; rm parser.tab.* lex.yy.*

# CC=gcc
# CFLAGS=-Wall -pedantic
# CLIBS=-lfl

# PARSEFLAGS=--verbose --report=all --yacc --defines=parser.tab.c --output=parser.tab.h

# all: main.c
# 	bison --verbose --report=all -d parser.y
# 	flex -l lexer.l
# 	${CC} ${CFLAGS} $^ -o GoLite ${CLIBS}

# clean:
# 	rm lex.yy.c *.output *.tab.?

# CC = gcc

# CFLAGS = -g

# main: tree.c y.tab.c lex.yy.c main.c 
# 	${CC} ${CFLAGS} $^ -o GoLite -lfl

# y.tab.c y.tab.h: parser.y
# 	bison --yacc -d parser.y

# lex.yy.c: lexer.l y.tab.h
# 	flex lexer.l

# clean: 
# 	rm y.tab.* lex.yy.*; rm *.o; rm GoLite
