all:
	flex -osrc/lex.gr.c -P gr src/gr.l
	gcc -g src/genASDR.c src/lex.gr.c src/grammar.c src/list.c src/asdr.c -o bin/genADFR -g
	rm src/*.gr.*
