
all: bin/cTypeCompiler

bin/cTypeCompiler: c_type_compiler.c tool_output/c_type_gram.c tool_output/c_type_lex.c
	gcc -O2 -s -o bin/cTypeCompiler c_type_compiler.c sqlite3/sqlite3.o -Wall -Winline -Wignored-attributes -ldl

tool_output/c_type_gram.c: tool/lemon c_type_gram.y
	./tool/lemon c_type_gram.y -s -dtool_output

tool/lemon: tool/lemon.c tool/lempar.c
	gcc -O2 tool/lemon.c -o tool/lemon

tool/lemon.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lemon.c > tool/lemon.c

tool/lempar.c:
	curl https://raw.githubusercontent.com/sqlite/sqlite/master/tool/lempar.c > tool/lempar.c

tool_output/c_type_lex.c: c_type_lex.re
	re2c -W c_type_lex.re -o tool_output/c_type_lex.c

clean:
	rm bin/cTypeCompiler
