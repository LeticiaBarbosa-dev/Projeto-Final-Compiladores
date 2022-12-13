%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "lbscript-lib.h"

int yylex();
void yyerror (char *s){
	printf("%s\n", s);
}

%}

%union{
	float flo;
	int fn;
	int inter;
	char str[50];
	Ast *a;
}

%token <flo>NUM
%token <flo>FLOAT
%token <inter>INT
%token <str>TEXTO
%token <str>VARS
%token WHILE
%token COMECA TERMINA IF ELSE PRINT READ PRINT_S XP RAIZ STRING READ_S ESCREVA_TEXTO
%token <fn> CMP


%right '='
%left '+' '-'
%left '*' '/'
%left CMP
%left XP RAIZ
%left ICC DECC

%type <a> exp list stmt prog exp1

%nonassoc IFX VARPREC DECLPREC NEG VETOR

%%

val: COMECA prog TERMINA
	;

prog: stmt 		{eval($1);}  /*Inicia e execução da árvore de derivação*/
	| prog stmt {eval($2);}	 /*Inicia e execução da árvore de derivação*/
	;
	
/*Funções para análise sintática e criação dos nós na AST*/	
/*Verifique q nenhuma operação é realizada na ação semântica, apenas são criados nós na árvore de derivação com suas respectivas operações*/
	
stmt: IF '(' exp ')' '{' list '}' %prec IFX
		{
			$$ = newflow('I', $3, $6, NULL);
		}
	
	| IF '(' exp ')' '{' list '}' ELSE '{' list '}'
		{
			$$ = newflow('I', $3, $6, $10);
		}

	| WHILE '(' exp ')' '{' list '}'
		{
			$$ = newflow('W', $3, $6, NULL);
		}
	| VARS '=' exp
		{
			$$ = newasgn($1,$3);
		}
	| PRINT '(' exp ')'
		{
			$$ = newast('C',$3,NULL);
		}
	| PRINT_S '(' exp1 ')'
		{
			$$ = newast('Y',$3,NULL);
		}
	| READ_S '('VARS')'
		{
			$$ = newvari('Z',$3);
		}
	| ESCREVA_TEXTO '(' exp1 ')' { $$ = newast('p',$3,NULL);}
	| READ '(' VARS ')' {$$ = newvari('G', $3);}
	| FLOAT VARS
		{
			$$ = newvari('V',$2);
		}
	| INT VARS
		{
			$$ = newvari('T',$2);
		}
	| STRING VARS
		{
			$$ = newvari('B', $2);
		}
	| INT VARS '['NUM']' { $$ = newarray('a', $2, $4);}
	| FLOAT VARS '['NUM']' { $$ = newarray('a', $2, $4);}
	| VARS '['NUM']' '=' exp {$$ = newasgn_a($1,$6,$3);}

;	

list:	  stmt{$$ = $1;}
		| list stmt { $$ = newast('L', $1, $2);	}
		;
	
exp: 
	 exp '+' exp {$$ = newast('+',$1,$3);}		/*Expressões matemáticas*/
	|exp '-' exp {$$ = newast('-',$1,$3);}
	|exp '*' exp {$$ = newast('*',$1,$3);}
	|exp '/' exp {$$ = newast('/',$1,$3);}
	|XP '(' exp '^' exp ')' {$$ = newast('X', $3, $5);}
	|ICC '(' exp ')' {$$ = newast('E', $3, NULL);}
	|DECC '(' exp ')' {$$ = newast('D', $3, NULL);}
	|RAIZ '(' exp ')' {$$ = newast('R', $3, NULL);}
	|exp CMP exp {$$ = newcmp($2,$1,$3);}		/*Testes condicionais*/
	|'(' exp ')' {$$ = $2;}
	|'-' exp %prec NEG {$$ = newast('M',$2,NULL);}
	|NUM {$$ = newnumInt($1);}		
	|TEXTO {$$ = newString($1);}
	|INT {$$ = newnumInt($1);}						/*token de um número*/
	|FLOAT {$$ = newnumFloat($1);}		
	|VARS %prec VETOR {$$ = newValorVal($1);}
	|VARS '['NUM']' {$$ = newValorVal_a($1, $3);}			/*token de uma variável*/
	;

exp1: 
	VARS {$$ = newValorValS($1);}		
	| TEXTO {$$ = newString($1);}
	;
%%

#include "lex.yy.c"

int main(){
	
	yyin=fopen("contador.lbscript","r");
	//yyin=fopen("areaFormasGeometricas.lbscript","r");
	// yyin=fopen("jurosSimples.lbscript","r");
	yyparse();
	yylex();
	fclose(yyin);
return 0;
}

