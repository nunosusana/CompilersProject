lex gocompiler.l
bison -y -d -v gocompiler.y
clang lex.yy.c y.tab.c ast.c tabsym.c semantics.c -g -o gocompiler

