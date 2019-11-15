
all: cTypeCompiler

cTypeCompiler: c_type_compiler.c c_type_gram.c c_type_lex.c
	gcc -O2 -s -o cTypeCompiler c_type_compiler.c sqlite3/sqlite3.o -Wall -Winline -Wignored-attributes -ldl

c_type_gram.c: lemon c_type_gram.y lempar.c
	./lemon c_type_gram.y -s

lemon: lemon.c
	gcc -O2 lemon.c -o lemon

c_type_lex.c: c_type_lex.re
	re2c -W c_type_lex.re -o c_type_lex.c

clean:
	rm cTypeCompiler
