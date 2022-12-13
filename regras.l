%option noyywrap
%{
#include <string.h>
%}

NUM [0-9]+\.?[0-9]*
VARS [a-z]+[0-9]*
COMECA "INICIO"
TERMINA "FIM"
COMENTARIO_LINHA ['#']
MULT_COMMENT ['@@']
RETURN "retorne"
IF "se"
ELSE "senao"
WHILE "enquanto"
PRINT "imprima"
PRINT_S "imprimaStr"
READ "leia"
READ_S "leiaStr"
RAIZ "RAIZ"
XP "XP"
FLOAT "FLOAT"
INT "INT"
STRING "STRING"
ICC "ICC"
DECC "DECC"
ESCREVA_TEXTO "escrevaTexto"
TEXTO (\".+*\")
	

%%
{COMECA} {return COMECA;}
{TERMINA} {return TERMINA;}
{ICC} {return ICC;}
{DECC} {return DECC;}
{COMENTARIO_LINHA}.* {;}
{MULT_COMMENT}(.*|"\n")*{MULT_COMMENT} {;}
{RETURN} {;}
{IF} {return IF;}
{ELSE} {return ELSE;}
{WHILE} {return WHILE;}
{PRINT} {return PRINT;}
{PRINT_S} {return PRINT_S;}
{READ} {return READ;}
{READ_S} {return READ_S;}
{RAIZ} {return RAIZ;}
{XP} {return XP;}
{FLOAT} {return FLOAT;}
{INT} {return INT;}
{STRING} {return STRING;}
{ESCREVA_TEXTO} {return ESCREVA_TEXTO;}

[" "\n\t\r]* {;}

{VARS} {
	strcpy(yylval.str,yytext);
	return VARS;
	}

{NUM} {
	yylval.flo = atof(yytext);
	return NUM;
	}

{TEXTO} {
	int i; 
	for(i = 0; i < strlen(yytext); i++){
		yytext[i] = yytext[i+1];
	}
	yytext[i-2] = '\0';
	strcpy(yylval.str, yytext);
	//printf("%s .l\n", yytext);
	return TEXTO;
	}

[-+()=/*^{};\[\]] {return *yytext;}
			

">" {yylval.fn = 1; return CMP; }
"<" {yylval.fn = 2; return CMP; }
"<>" {yylval.fn = 3; return CMP; }
"==" {yylval.fn = 4; return CMP; }
">=" {yylval.fn = 5; return CMP; }
"<=" {yylval.fn = 6; return CMP; }

[\ \n\t]*	{;}
. {printf("(%s) Ignorado\n",yytext);}
%%