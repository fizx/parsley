yacc -d -t parser.y
lex -o scanner.yy.c scanner.l
gcc -std=c99 -lfl -L/opt/local/lib -I/opt/local/include -ljson -largp obstack.c printbuf.c kstring.c y.tab.c scanner.yy.c dexter.c dexterc_main.c -o dexterc