#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>	

    typedef struct vars{//estrutura de uma variável
		int nodetype;
		int type;
		int tipo;
		char name[50];
		double valor;
		char valors[50];
		int valor2;
		double *vetor;
		struct vars * prox;
	}VARI;
    
    	
	//insere uma nova variável na lista de variáveis
	VARI *ins(VARI*l,char n[]){
		VARI*new =(VARI*)malloc(sizeof(VARI));
		strcpy(new->name,n);
		new->prox = l;
		return new;
	}

	VARI *ins_a(VARI*l, char n[], int size) {
		VARI*new =(VARI*)malloc(sizeof(VARI));
		strcpy(new->name,n);
		new->vetor = (double*)malloc(size * sizeof(double));
		new->prox = l;
		new->nodetype = 3;
		return new;
	}
	
	//busca uma variável na lista de variáveis
	VARI *srch(VARI*l,char n[]){
		VARI*aux = l;
		while(aux != NULL){
			if(strcmp(n,aux->name)==0)
				return aux;
			aux = aux->prox;
		}
		return aux;
	}
	
		
/*O node type serve para indicar o tipo de nó que está na árvore. Isso serve para a função eval() entender o que realizar naquele nó*/
typedef struct ast { /*Estrutura de um nó*/
	int nodetype;
	struct ast *l; /*Esquerda*/
	struct ast *r; /*Direita*/
}Ast; 

typedef struct numval { /*Estrutura de um número*/
	int nodetype;
	double number;
}Numval;

typedef struct varval { /*Estrutura de um nome de variável, nesse exemplo uma variável é um número no vetor var[26]*/
	int nodetype;
	char var[50];
	int size;
}Varval;


typedef struct flow { /*Estrutura de um desvio (if/else/while)*/
	int nodetype;
	Ast *cond;		/*condição*/
	Ast *tl;		/*then, ou seja, verdade*/
	Ast *el;		/*else*/
}Flow;

typedef struct symasgn { /*Estrutura para um nó de atribuição. Para atrubior o valor de v em s*/
	int nodetype;
	char s[50];
	Ast *v;
	int pos;
}Symasgn;

typedef struct strVal { /*Estrutura de a string*/
	int nodetype;
	char value[50];
}StrVal;

/*Variáveis*/
VARI *l1; /*Lista de números reais*/
VARI *l2; /*Lista de números inteiros*/
VARI *l3; /*Lista de strings*/
VARI *aux;

Ast * newast(int nodetype, Ast *l, Ast *r){ /*Função para criar um nó*/

	Ast *a = (Ast*) malloc(sizeof(Ast));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	a->l = l;
	a->r = r;
	return a;
}


Ast * newString(char str[]) { /*Função de que cria uma nova string*/
	StrVal *a = (StrVal*) malloc(sizeof(StrVal));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'J';
	strcpy(a->value, str);
	return (Ast*)a;
}
 
Ast * newvari(int nodetype, char nome[50]) {			/*Função de que cria uma nova variável*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;;
	strcpy(a->var,nome);
	return (Ast*)a;
}

Ast * newarray(int nodetype, char nome[50], int tam) {			/*Função de que cria uma nova variável*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = nodetype;
	strcpy(a->var,nome);
	a->size = tam;
	return (Ast*)a;
}	

Ast * newnumFloat(double d) {			/*Função que cria um novo número*/
	Numval *a = (Numval*) malloc(sizeof(Numval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'K';
	a->number = d;
	return (Ast*)a;
}	

Ast * newnumInt(int d) {			/*Função que cria um novo número*/
	Numval *a = (Numval*) malloc(sizeof(Numval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'K';
	a->number = d;
	return (Ast*)a;
}	
	
Ast * newflow(int nodetype, Ast *cond, Ast *tl, Ast *el){ /*Função que cria um nó de if/else/while*/
	Flow *a = (Flow*)malloc(sizeof(Flow));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = nodetype;
	a->cond = cond;
	a->tl = tl;
	a->el = el;
	return (Ast *)a;
}

Ast * newcmp(int cmptype, Ast *l, Ast *r){ /*Função que cria um nó para testes*/
	Ast *a = (Ast*)malloc(sizeof(Ast));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '0' + cmptype; /*Para pegar o tipo de teste, definido no arquivo.l e utilizar na função eval()*/
	a->l = l;
	a->r = r;
	return a;
}

Ast * newasgn(char s[50], Ast *v) { 		/*Função para um nó de atribuição*/
	Symasgn *a = (Symasgn*)malloc(sizeof(Symasgn));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '=';
	strcpy(a->s,s);
	//a->s = s; /*Símbolo/variável*/
	a->v = v; /*Valor*/
	return (Ast *)a;
}

Ast * newasgn_a(char s[50], Ast *v, int indice) { /*Função para um nó de atribuição*/
	Symasgn *a = (Symasgn*)malloc(sizeof(Symasgn));
	if(!a) {
		printf("out of space");
	exit(0);
	}
	a->nodetype = '=';
	strcpy(a->s,s);
	a->v = v; /*Valor*/
	a->pos = indice;
	return (Ast *)a;
}

Ast * newValorVal(char s[50]) { 		/*Função que recupera o nome/referência de uma variável, neste caso o número*/
	
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'N';
	strcpy(a->var,s);
	return (Ast*)a;
	
}

Ast * newValorVal_a(char s[], int index) { /*Função que recupera o nome/referência de uma variável, neste caso o número*/
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'O';
	strcpy(a->var,s);
	a->size = index;
	
	return (Ast*)a;
}	


Ast * newValorValS(char s[50]) { /*Função que recupera o nome/referência de uma variável, neste caso o número*/
	
	Varval *a = (Varval*) malloc(sizeof(Varval));
	if(!a) {
		printf("out of space");
		exit(0);
	}
	a->nodetype = 'Q';
	strcpy(a->var,s);
	return (Ast*)a;
}

char * eval2(Ast *a) { /*Função que executa operações a partir de um nó*/
		VARI *aux1;
		char *v2;
		
			switch(a->nodetype) {
			
			case 'Q':
				aux1 = srch(l3,((Varval *)a)->var);
				return aux1->valors;
				break;

			default: printf("internal error: bad node %c\n", a->nodetype);
					break;
		}
		
	return v2;
}



double eval(Ast *a) { /*Função que executa operações a partir de um nó*/
	double v;
	double tt;
	char v1[50]; // string
	int inteiros; // inteiros
	char *v2;
	VARI * aux1;
	if(!a) {
		printf("internal error, null eval");
		return 0.0;
	}
	switch(a->nodetype) {
		case 'K': v = ((Numval *)a)->number; break; 	/*Recupera um número*/
		case 'O': 
			aux1 = srch(l1,((Varval *)a)->var);
			v = aux1->vetor[((Varval *)a)->size];
			break;
		case 'N': 
			aux1 = srch(l1,((Varval *)a)->var);
			if(aux1 != NULL) {
				v = aux1->valor;
			} else {
				aux1 = srch(l2, ((Varval *)a)->var);
				if(aux1 == NULL) {
					v = aux1->valor2;
				}
			}
			break;
		case '+': v = eval(a->l) + eval(a->r); break;	/*Operações "árv esq   +   árv dir"*/
		case '-': v = eval(a->l) - eval(a->r); break;	/*Operações*/
		case '*': v = eval(a->l) * eval(a->r); break;	/*Operações*/
		case '/': v = eval(a->l) / eval(a->r); break; /*Operações*/
		case 'E': v = eval(a->l) + 1; break; /*Incremento em 1*/
		case 'D': v = eval(a->l) - 1; break; /*Decremento em 1*/
		case 'M': v = -eval(a->l); break;				/*Operações, número negativo*/
		case 'X': v = pow(eval(a->l), eval(a->r)); break; /*Operações de exponenciação*/
		case 'R': v = sqrt(eval(a->l)); break; /*Operações de raiz*/
	
		case '1': v = (eval(a->l) > eval(a->r))? 1 : 0; break;	/*Operações lógicas. "árv esq   >   árv dir"  Se verdade 1, falso 0*/
		case '2': v = (eval(a->l) < eval(a->r))? 1 : 0; break;
		case '3': v = (eval(a->l) != eval(a->r))? 1 : 0; break;
		case '4': v = (eval(a->l) == eval(a->r))? 1 : 0; break;
		case '5': v = (eval(a->l) >= eval(a->r))? 1 : 0; break;
		case '6': v = (eval(a->l) <= eval(a->r))? 1 : 0; break;
		
		case '=':
			// inicialmente verificar se a variável existe
			v = eval(((Symasgn *)a)->v); /*Recupera o valor*/
			aux = srch(l1,((Symasgn *)a)->s);

			if (aux == NULL){
				aux = srch(l2, ((Symasgn *)a)->s);
				if(aux == NULL) {
					printf("Variável não declarada: %s\n", ((Symasgn *)a)->s);
				} else {
					aux->valor2 = v;
				}
			} else {
				aux->valor = v;			
			}

			if(aux->nodetype == 3){
				aux->vetor[((Symasgn *)a)->pos] = v; //inserção no vetor
			}
		
			break;
		
		case 'I':						/*CASO IF*/
			if (eval(((Flow *)a)->cond) != 0) {	/*executa a condição / teste*/
				if (((Flow *)a)->tl)		/*Se existir árvore*/
					v = eval(((Flow *)a)->tl); /*Verdade*/
				else
					v = 0.0;
			} else {
				if( ((Flow *)a)->el) {
					v = eval(((Flow *)a)->el); /*Falso*/
				} else
					v = 0.0;
				}
			break;
			
		case 'W':
			v = 0.0;
			if( ((Flow *)a)->tl) {
				while( eval(((Flow *)a)->cond) != 0){
					v = eval(((Flow *)a)->tl);
				}
			}
		break;
			
		case 'L': 	eval(a->l);
					v = eval(a->r);

					break; /*Lista de operções em um bloco IF/ELSE/WHILE. Assim o analisador não se perde entre os blocos*/

		case 'C': 	v = eval(a->l);		/*Recupera um valor*/
					printf ("%.2lf\n", v); 
					break;  /*Função que imprime um valor float*/
		
		case 'V': 	l1 = ins(l1,((Varval*)a)->var);
					break; // FLOAT
		
		case 'T': 	l2 = ins(l2,((Varval*)a)->var);
					break; // INTEIRO

		case 'B': 	l3 = ins(l3,((Varval*)a)->var);
					break; // STRING

		case 'Y':	
					v2 = eval2(a->l);		/*Recupera um valor STR*/
					printf ("%s\n",v2);  /*Função que imprime um valor (string)*/
					//printf ("imprimiu\n");
					break;

		case 'Z': 	scanf("%s", v1);
					aux1 = srch(l3, ((Varval *)a)-> var);
					strcpy(aux1 ->valors,v1);
					break;
		case 'p': 	
					printf("%s\n", ((StrVal *)a->l)->value);
					break;

		case 'a':	
					l1 = ins_a(l1,((Varval*)a)->var,((Varval*)a)->size);
					break;

		case 'G': 	scanf("%lf", &v);
					aux1 = srch(l1, ((Varval *)a)-> var);
					aux1 -> valor = v;				
					break;
		default: printf("internal error: bad node %c\n", a->nodetype);
				
	}
	return v;
}
