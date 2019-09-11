	/*
	Nuno Miguel Pereira Susana 	2010159256
	Rúben Telmo Domingues Leal	2011181710
	*/

%{
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "semantics.h"
#include "tabsym.h"
#include "y.tab.h"

int flag = 0;
int flag_sintax = 0;
int flag_semantics = 0;
int l = 1, c = 1,c_bn;
int l_str,c_str;
int l_comment,c_comment;
int sc_flag = 0;
char buf[2048];
int pos_str=0;
int erro=0;
int erro_sintaxe=0;
struct node *pai_de_todos;
struct node *first;
%}
%X COMMENT STR

exp				[eE]([-+])?([0-9]+)

BLANKID  			"_"
ID				[a-zA-Z_][a-zA-Z0-9_]*

INTLIT				((("0x"|"0X")([a-f]|[A-F]|[0-9])+)|([0-9]+)|[0-7]+)
	
REALLIT				(([0-9]+".")([0-9]*({exp}([0-9]*))?)?|"."[0-9]+{exp}?|[0-9]+{exp})
	

STRLIT 				\"

SEMICOLON  			";"
PACKAGE 			"package"
RETURN 				"return"
AND 				"&&"
ASSIGN 				"="
STAR 				"*" 
COMMA 				"," 
DIV 				"/"
EQ 				"=="
GE 				">="
GT 				">"
LBRACE 				"{" 
LE 				"<="
LPAR 				"(" 
LSQ 				"["
LT 				"<"
MINUS 				"-"
MOD 				"%"
NE 				"!="
NOT 				"!"
OR 				"||"
PLUS 				"+"
RBRACE 				"}" 
RPAR 				")" 
RSQ 				"]" 
ELSE 				"else" 
FOR 				"for" 
IF 				"if"
VAR 				"var"
INT 				"int"
FLOAT32 			"float32"
BOOL 				"bool"
STRING 				"string"
PRINT 				"fmt.Println"
PARSEINT 			"strconv.Atoi"
FUNC 				"func"
CMDARGS 			"os.Args"
RESERVED		 	"++"|"--"|"break"|"default"|"interface"|"select"|"case"|"defer"|"go"|"map"|"struct"|"chan"|"goto"|"switch"|"const"|"fallthrough"|"range"|"type"|"continue"|"import"
LINECOMMENT			"//"[^\n\r\f]*
%%

"/*"				{BEGIN COMMENT; l_comment = l; c_comment = c; c+=yyleng;}
<COMMENT><<EOF>>		{printf("Line %d, column %d: unterminated comment\n", l_comment, c_comment); BEGIN 0;}
<COMMENT>\n|\r		 	{l++; c=1; if(sc_flag && flag) printf("SEMICOLON\n");if(sc_flag){ yylval.Node=addNext(first,newNode("SEMICOLON",strdup(yytext), l, c));  sc_flag = 0; return SEMICOLON;} }
<COMMENT>\f			{l++; c++;}
<COMMENT>"*/"			{c+=yyleng; BEGIN 0;}
<COMMENT>.			{c+=yyleng;}

{STRLIT}			{memset(buf,0,strlen(buf)); sc_flag = 1; erro=0; buf[0]='"'; l_str=l; c_str=c; c+=yyleng; BEGIN STR;}
<STR>\"				{strcat(buf, yytext); if(erro) sc_flag = 0; if(flag && !erro) printf("STRLIT(%s)\n",buf); if(!erro){yylval.Node=addNext(first,newNode("StrLit",strdup(buf), l, c)); c+=yyleng; erro=0; BEGIN 0; return STRLIT;} c+=yyleng;  erro=0; BEGIN 0;}
<STR>\\[^nrft\\\"]		{printf("Line %d, column %d: invalid escape sequence (%s)\n",l,c,yytext); c+=yyleng; sc_flag=0; erro=1;}
<STR>\\[nrft\\\"]		{c+=yyleng; strcat(buf, yytext);}
<STR>.				{c+=yyleng; strcat(buf, yytext);}
<STR>\n|\r			{printf("Line %d, column %d: unterminated string literal\n",l_str,c_str); l++;c=1; erro=0; sc_flag=0; BEGIN 0;}

{LINECOMMENT}			{c+=yyleng;}
{BLANKID}			{if(flag) printf("BLANKID\n");c+=yyleng;  yylval.Node=addNext(first,newNode("BlankId",strdup(yytext), l, c-yyleng)); return BLANKID;}

\n|\r				{c_bn=c; c=1;l++; if(sc_flag && flag) printf("SEMICOLON\n");if(sc_flag) { yylval.Node=addNext(first,newNode("SEMICOLON",strdup(" "), l, c)); sc_flag = 0; return SEMICOLON;}}

\f				{c++; l++;if(sc_flag && flag) printf("SEMICOLON\n");if(sc_flag) {  yylval.Node=addNext(first,newNode("SEMICOLON",strdup(""), l, c)); sc_flag = 0; return SEMICOLON;}}
" "|\t				{c++;}

<<EOF>>		{c_bn=c; c=1;l++; if(sc_flag && flag) printf("SEMICOLON\n");if(sc_flag) { yylval.Node=addNext(first,newNode("SEMICOLON",strdup(" "), l, c)); sc_flag = 0; return SEMICOLON;} yyterminate();}
{SEMICOLON}  		{if(flag) printf("SEMICOLON\n"); sc_flag = 0; c+=yyleng; yylval.Node=addNext(first,newNode("SEMICOLON",strdup(yytext), l, c-yyleng)); return SEMICOLON;}
{PACKAGE} 			{if(flag) printf("PACKAGE\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Package",strdup(yytext), l, c-yyleng)); return PACKAGE;}
{RETURN} 			{if(flag) printf("RETURN\n"); c+=yyleng; sc_flag=1; yylval.Node=addNext(first,newNode("Return",strdup(yytext), l, c-yyleng)); return RETURN;}
{AND} 				{if(flag) printf("AND\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("And",strdup(yytext), l, c-yyleng)); return AND;}
{ASSIGN} 			{if(flag) printf("ASSIGN\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Assign",strdup(yytext), l, c-yyleng)); return ASSIGN;}
{STAR} 				{if(flag) printf("STAR\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Mul",strdup(yytext), l, c-yyleng)); return STAR;}
{COMMA} 			{if(flag) printf("COMMA\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("COMMA",strdup(yytext), l, c-yyleng)); return COMMA;}
{DIV} 				{if(flag) printf("DIV\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Div",strdup(yytext), l, c-yyleng)); return DIV;}
{EQ} 				{if(flag) printf("EQ\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Eq",strdup(yytext), l, c-yyleng)); return EQ;}
{GE} 				{if(flag) printf("GE\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Ge",strdup(yytext), l, c-yyleng)); return GE;}
{GT} 				{if(flag) printf("GT\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Gt",strdup(yytext), l, c-yyleng)); return GT;}
{LBRACE} 			{if(flag) printf("LBRACE\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Block","", l, c-yyleng)); return LBRACE;}
{LE} 				{if(flag) printf("LE\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Le",strdup(yytext), l, c-yyleng)); return LE;}
{LPAR} 				{if(flag) printf("LPAR\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("LPAR",strdup(yytext), l, c-yyleng)); return LPAR;}
{LSQ} 				{if(flag) printf("LSQ\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("LSQ",strdup(yytext), l, c-yyleng)); return LSQ;}
{LT} 				{if(flag) printf("LT\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Lt",strdup(yytext), l, c-yyleng)); return LT;}
{MINUS} 			{if(flag) printf("MINUS\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Sub",strdup(yytext), l, c-yyleng)); return MINUS;}
{MOD} 				{if(flag) printf("MOD\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Mod",strdup(yytext), l, c-yyleng)); return MOD;}
{NE} 				{if(flag) printf("NE\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Ne",strdup(yytext), l, c-yyleng)); return NE;}
{NOT} 				{if(flag) printf("NOT\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Not",strdup(yytext), l, c-yyleng)); return NOT;}
{OR} 				{if(flag) printf("OR\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Or",strdup(yytext), l, c-yyleng)); return OR;}
{PLUS} 				{if(flag) printf("PLUS\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Add",strdup(yytext), l, c-yyleng)); return PLUS;}
{RBRACE} 			{if(flag) printf("RBRACE\n"); c+=yyleng; sc_flag = 1; yylval.Node=addNext(first,newNode("Block","", l, c-yyleng)); return RBRACE;}
{RPAR} 				{if(flag) printf("RPAR\n"); c+=yyleng; sc_flag = 1; yylval.Node=addNext(first,newNode("RPAR",strdup(yytext), l, c-yyleng)); return RPAR;}
{RSQ} 				{if(flag) printf("RSQ\n"); c+=yyleng; sc_flag = 1; yylval.Node=addNext(first,newNode("RSQ",strdup(yytext), l, c-yyleng)); return RSQ;}
{ELSE} 				{if(flag) printf("ELSE\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Else",strdup(yytext), l, c-yyleng)); return ELSE;}
{FOR} 				{if(flag) printf("FOR\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("For",strdup(yytext), l, c-yyleng)); return FOR;}
{IF} 				{if(flag) printf("IF\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("If",strdup(yytext), l, c-yyleng)); return IF;}
{VAR} 				{if(flag) printf("VAR\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Var",strdup(yytext), l, c-yyleng)); return VAR;}
{INT} 				{if(flag) printf("INT\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Int",strdup(yytext), l, c-yyleng)); return INT;}
{FLOAT32} 			{if(flag) printf("FLOAT32\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Float32",strdup(yytext), l, c-yyleng)); return FLOAT32;}
{BOOL} 				{if(flag) printf("BOOL\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Bool",strdup(yytext), l, c-yyleng)); return BOOL;}
{STRING} 			{if(flag) printf("STRING\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("String",strdup(yytext), l, c-yyleng)); return STRING;}
{PRINT} 			{if(flag) printf("PRINT\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Print",strdup(yytext), l, c-yyleng)); return PRINT;}
{PARSEINT} 			{if(flag) printf("PARSEINT\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("ParseInt",strdup(yytext), l, c-yyleng)); return PARSEINT;}
{FUNC} 				{if(flag) printf("FUNC\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("FuncHeader",strdup(yytext), l, c-yyleng)); return FUNC;}
{CMDARGS} 			{if(flag) printf("CMDARGS\n"); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("CmdArgs",strdup(yytext), l, c-yyleng)); return CMDARGS;}
{RESERVED}			{if(flag) printf("RESERVED(%s)\n", yytext); c+=yyleng; sc_flag = 0; yylval.Node=addNext(first,newNode("Reserved",strdup(yytext), l, c-yyleng)); return RESERVED;}
{INTLIT}			{if(flag) printf("INTLIT(%s)\n",yytext);c+=yyleng;sc_flag = 1; yylval.Node=addNext(first,newNode("IntLit",strdup(yytext), l, c-yyleng)); return INTLIT;}
{REALLIT}			{if(flag) printf("REALLIT(%s)\n",yytext);c+=yyleng;sc_flag = 1; yylval.Node=addNext(first,newNode("RealLit",strdup(yytext), l, c-yyleng)); return REALLIT;}
{ID}				{if(flag) printf("ID(%s)\n",yytext); sc_flag = 1; c+=yyleng; yylval.Node=addNext(first,newNode("Id",strdup(yytext), l, c-yyleng)); return ID;}
. 				{printf("Line %d, column %d: illegal character (%c)\n",l,c,yytext[0]); c+=yyleng; sc_flag = 0;}

%%
int main(int argc, char **argv){
	semantics_error = 0; 
	first = newNode("NodeList","", 0, 0);
	if(argc>1){
		if(strcmp(argv[1],"-l")==0){
			flag = 1;
			while(yylex());
		}else if(strcmp(argv[1],"-t")==0){
			flag_sintax=1;
			yyparse();
		}else if(strcmp(argv[1],"-s")==0){
			flag_semantics=1;
			yyparse();
		}
	}else{
		yyparse();
	}
	
	if(flag_sintax && !erro_sintaxe)
		printAST(pai_de_todos,0);

	if(!erro_sintaxe)
		doSemantics(pai_de_todos);
	if(flag_semantics && !erro_sintaxe){	
		print_tables(tabelas);
		printAST_annoted(pai_de_todos,0);
		
	}
	freeTabs(tabelas);
	//freeTabs(tabelas);
	freeAllNodes(first);
	return 0;
}

int yywrap(){
	if(flag && sc_flag){
		printf("SEMICOLON\n");
		sc_flag = 0;
	}
	if(sc_flag & flag_sintax){
		sc_flag = 0;
		yylval.Node=newNode("SEMICOLON",strdup(yytext), 0, 0);
		return SEMICOLON;
	}
	

	return 1;
}

void yyerror (char *s) {
	if(yytext[0]=='"')
		printf("Line %d, column %d: %s: %s\n",l_str,c_str,s,buf);
	else if(yytext[0]==0)
		printf("Line %d, column %d: %s: %s\n",l-1,c_bn,s,yytext);
	else
		printf("Line %d, column %d: %s: %s\n",l,c-yyleng,s,yytext);

}
