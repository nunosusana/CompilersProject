%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "ast.h"

int yylex(void);
void yyerror (char *s);
extern struct node *pai_de_todos;
extern struct node *first;
extern int erro_sintaxe;
%}

%union{
	struct node* Node;
}


%token <Node> PACKAGE FUNC SEMICOLON VAR LPAR RPAR COMMA BLANKID ASSIGN PARSEINT CMDARGS LSQ RSQ IF ELSE LBRACE RBRACE PRINT RETURN NOT NE LE GE LT GT EQ MINUS PLUS DIV STAR MOD INT BOOL STRING FLOAT32 FOR OR AND INTLIT REALLIT STRLIT ID RESERVED

%type <Node> MultiBraces Program Declarations VarDeclaration VarSpec MultId FuncDeclaration Parameters MultIdType Type FuncBody VarsAndStatements Statement MultStatement ParseArgs FuncInvocation MultExpr Expr NotMinusPlus ExprOps

%left COMMA
%right ASSIGN
%left OR
%left AND
%left GT GE LT LE NE EQ
%left PLUS MINUS
%left STAR DIV MOD
%right NOT

%%
Program: PACKAGE ID SEMICOLON Declarations			{$$ = pai_de_todos = addNext(first,addNode("Program", 0, 0, 1, $4));}
	| PACKAGE ID SEMICOLON							{$$ = pai_de_todos = addNext(first,addNode("Program", 0, 0, 0));}
	;
Declarations: VarDeclaration SEMICOLON				{$$ = $1;}
	| FuncDeclaration SEMICOLON						{$$ = $1;}
	| Declarations VarDeclaration SEMICOLON			{$$ = packNodes(2, $1, $2);}
	| Declarations FuncDeclaration SEMICOLON		{$$ = packNodes(2, $1, $2);}
	
	;
VarDeclaration: VAR VarSpec					{$$ = $2;}
	| VAR LPAR VarSpec SEMICOLON RPAR		{$$ = $3;}
	;
VarSpec: ID MultId Type			{$$ = addMultipleId(packNodes(2,$1,$2),$3);}
	| ID Type					{$$ = addNode("VarDecl", 0, 0, 2, $2,$1); addNext(first,$$);}
	;
MultId: COMMA ID			{$$ = $2;}
	| MultId COMMA ID 		{$$ = packNodes(2,$1,$3);}
	;
FuncDeclaration: FUNC ID LPAR Parameters RPAR FuncBody	{$$ = addNode("FuncDecl", 0, 0, 2, addNode2($1,2,$2,addNext(first,addNode("FuncParams", 0, 0, 1,$4))),$6); addNext(first,$$);}
	| FUNC ID LPAR RPAR Type FuncBody					{$$ = addNode("FuncDecl", 0, 0, 2, addNode2($1,3,$2,$5,addNext(first,addNode("FuncParams", 0, 0, 0))), $6); addNext(first,$$);}
	| FUNC ID LPAR Parameters RPAR Type FuncBody		{$$ = addNode("FuncDecl", 0, 0, 2, addNode2($1,3,$2,$6,addNext(first,addNode("FuncParams", 0, 0, 1,$4))), $7); addNext(first,$$);}
	| FUNC ID LPAR RPAR FuncBody						{$$ = addNode("FuncDecl", 0, 0, 2, addNode2($1,2,$2,addNext(first,addNode("FuncParams", 0, 0, 0))), $5); addNext(first,$$);}
	;
Parameters: ID Type 					{$$ = addNode("ParamDecl", 0, 0, 2, $2, $1); addNext(first,$$);}
	| ID Type MultIdType				{$$ = packNodes(2,addNext(first,addNode("ParamDecl", 0, 0, 2, $2, $1)), $3);}
	;
MultIdType: COMMA ID Type						{$$ = addNode("ParamDecl", 0, 0, 2, $3, $2); addNext(first,$$);}
	| MultIdType COMMA ID Type 					{$$ = packNodes(2, $1, addNext(first,addNode("ParamDecl", 0, 0, 2, $4,$3)));}
	;
Type: INT								{$$ = $1;}
	| FLOAT32							{$$ = $1;}
	| BOOL								{$$ = $1;}
	| STRING							{$$ = $1;}
	;
FuncBody: LBRACE RBRACE								{$$ = addNode("FuncBody", 0, 0, 0); addNext(first,$$);}
	| LBRACE VarsAndStatements RBRACE				{$$ = addNode("FuncBody", 0, 0, 1, $2); addNext(first,$$);}
	;
VarsAndStatements: VarsAndStatements SEMICOLON		{$$ = $1;}
	| VarsAndStatements VarDeclaration SEMICOLON	{$$ = packNodes(2, $1, $2);}
	| VarsAndStatements Statement SEMICOLON			{$$ = packNodes(2, $1, $2);}
	| VarDeclaration SEMICOLON						{$$ = $1;}
	| Statement SEMICOLON							{$$ = $1;}
	| SEMICOLON										{$$ = addNode("Braces", 0, 0, 0); addNext(first,$$);}
	;
Statement: ID ASSIGN Expr									{$$ = addNode2($2, 2, $1, $3);}
	| LBRACE Statement SEMICOLON MultStatement RBRACE		{$1->data = "1"; $$ = addNode2($1, 2, $2, $4);}
	| LBRACE Statement SEMICOLON RBRACE						{$$ = $2;}
	| LBRACE MultiBraces RBRACE								{$$ = addNode("Braces", 0, 0, 0); addNext(first,$$);}
	| IF Expr LBRACE MultStatement RBRACE ELSE LBRACE MultStatement RBRACE 	{$3->data="0"; $7->data="0"; $$ = addNode2($1, 3, $2, addNode2($3,1, $4) , addNode2($7,1, $8));}
	| IF Expr LBRACE MultStatement RBRACE ELSE LBRACE RBRACE 	{$3->data="0"; $7->data="0"; $$ = addNode2($1, 3, $2, addNode2($3,1, $4), $7);}
	| IF Expr LBRACE MultStatement RBRACE						{$3->data="0"; $5->data="0"; $$ = addNode2($1, 3, $2, addNode2($3,1, $4), $5);}
	| IF Expr LBRACE RBRACE ELSE LBRACE MultStatement RBRACE 	{$3->data="0"; $6->data="0"; $$ = addNode2($1, 3, $2, $3 , addNode2($6,1, $7));}
	| IF Expr LBRACE RBRACE ELSE LBRACE RBRACE 		{$3->data="0"; $4->data="0"; $$ = addNode2($1, 3, $2, $3, $4);}
	| IF Expr LBRACE RBRACE							{$3->data="0"; $4->data="0"; $$ = addNode2($1, 3, $2, $3, $4);}
	| FOR Expr LBRACE MultStatement RBRACE			{$3->data="0"; $$ = addNode2($1, 2, $2, addNode2($3,1, $4));}
	| FOR Expr LBRACE RBRACE						{$3->data="0"; $$ = addNode2($1, 2, $2, $3);}
	| FOR LBRACE MultStatement RBRACE				{$2->data="0"; $$ = addNode2($1, 1, addNode2($2,1, $3));}
	| FOR LBRACE RBRACE								{$2->data="0"; $$ = addNode2($1, 1, $2);}
	| RETURN Expr									{$$ = addNode2($1, 1, $2);}
	| RETURN										{$$ = $1;}
	| ParseArgs										{$$ = $1;}
	| PRINT LPAR Expr RPAR							{$$ = addNode2($1,1, $3);}
	| PRINT LPAR STRLIT RPAR						{$$ = addNode2($1,1, $3);}
	| FuncInvocation								{$$ = $1;}
	| error											{erro_sintaxe=1; $$=addNode("Error", 0, 0, 0); addNext(first,$$);}
	;

MultiBraces: LBRACE MultiBraces SEMICOLON RBRACE	{;}
		| LBRACE Statement RBRACE					{;}
		| 											{;}
		;

MultStatement: Statement SEMICOLON				{$$=$1;}
	| MultStatement Statement SEMICOLON			{$$=packNodes(2,$1,$2);}
	;
ParseArgs: ID COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ Expr RSQ RPAR	{$$=addNode("ParseArgs", $5->line, $5->col, 2,$1,$9); addNext(first,$$);}
	|	ID COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR 					{erro_sintaxe=1; $$=addNode("Error", 0, 0, 0); addNext(first,$$);}
	;
FuncInvocation: ID LPAR MultExpr RPAR			{$$=addNode("Call", 0, 0, 2,$1,$3); addNext(first,$$);}
	| ID LPAR Expr RPAR							{$$=addNode("Call", 0, 0, 2,$1,$3); addNext(first,$$);}
	| ID LPAR RPAR								{$$=addNode("Call", 0, 0, 1,$1); addNext(first,$$);}
	| ID LPAR error RPAR						{erro_sintaxe=1; $$=addNode("Error", 0, 0, 0); addNext(first,$$);}
	;
MultExpr: Expr COMMA Expr				{$$=packNodes(2,$1,$3);}
	| MultExpr COMMA Expr				{$$=packNodes(2,$1,$3);}
	;
Expr: ExprOps			%prec NOT		{$$ = $1;}
	| NotMinusPlus Expr %prec NOT		{$$ = addNode2($1,1,$2);}
	| INTLIT							{$$ = $1;}
	| REALLIT							{$$ = $1;}
	| ID								{$$ = $1;}
	| FuncInvocation					{$$ = $1;}
	| LPAR error RPAR					{erro_sintaxe=1; $$=addNode("Error", 0, 0, 0); addNext(first,$$);}
	| LPAR Expr RPAR					{$$ = $2;}
	;
NotMinusPlus: NOT						{$$=$1;}
	| MINUS								{$$=addNodeData("Minus", strdup($1->data), $1->line, $1->col, 0); addNext(first,$$);}
	| PLUS								{$$=addNodeData("Plus", strdup($1->data), $1->line, $1->col, 0); addNext(first,$$);}
	;
ExprOps:Expr PLUS Expr			{$$=addNode2($2,2,$1,$3);}
	| Expr MINUS Expr			{$$=addNode2($2,2,$1,$3);}
	| Expr STAR Expr			{$$=addNode2($2,2,$1,$3);}
	| Expr DIV Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr MOD Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr LT Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr GT Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr EQ Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr NE Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr LE Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr GE Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr OR Expr				{$$=addNode2($2,2,$1,$3);}
	| Expr AND Expr				{$$=addNode2($2,2,$1,$3);}
	;
%%
