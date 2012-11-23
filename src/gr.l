%{
	#include <stdio.h>	
	#include <string.h>
	#include "grammar.h"
	#include "list.h"

	void controlLeft();
	void controlRight();
	void fin();
	int isSpace(char c);

	void setName(char * string);
	void addVN(char c);
	void addVT(char c);
	void setS(char c);
	void invalidGrammar();
	void irregularGrammar();
	void leftRecursivity();

	void addProduction();
	extern grammar* G;
	char left;
	listADT right;
	extern direction dir;
%}

%x VN_STATE
%x VT_STATE
%x S_STATE
%x P_STATE
%x P2_STATE
%x END_STATE

%%
<INITIAL>([a-zA-Z][^\ \t\n]*[\ \t\r\n]*=[\ \t\r\n]*\([\ \t\r\n]*\{) {
	G=newGrammar();
	setName(yytext);
	BEGIN VN_STATE;
};

<VN_STATE>([A-Z][\ \t\r\n]*,[\ \t\r\n]*) {
	addVN(yytext[0]);
};

<VN_STATE>([a-zA-Z]{2,}) {
	invalidGrammar();
}

<VN_STATE>([A-Z][\ \t\r\n]*\}[\ \t\r\n]*,[\ \t\r\n]*\{) {
	addVN(yytext[0]);
	BEGIN VT_STATE;
};

<VT_STATE>([a-z][\ \t\r\n]*,[\ \t\r\n]*) {
	addVT(yytext[0]);
};
<VT_STATE>([a-z][\ \t\r\n]*\}[\ \t\r\n]*,[\ \t\r\n]*) {
	addVT(yytext[0]);
	BEGIN S_STATE;
};

<S_STATE>([A-Z][\ \t\r\n]*,[\ \t\r\n]*\{[\ \t\r\n]*) {
	setS(yytext[0]);
	BEGIN P_STATE;
};

<P_STATE>([A-Z][\ \t\r\n]*-[\ \t\r\n]*>[\ \t\r\n]*) {
	left=yytext[0];
	right=newList(compareC);
	BEGIN P2_STATE;
};

<P_STATE>([A-Z][A-Z]+[\ \t\r\n]*-[\ \t\r\n]*>[\ \t\r\n]*) {
	invalidGrammar();
};

<P_STATE>([^A-Z\\a-z\->]) {
	invalidGrammar();
}

<P2_STATE>([A-Za-z\\]*[\ \t\r\n]*\|?[\ \t\r\n]*) {
	char c;
	int i=0;
	c=yytext[i++];
	reset(right);
	int first=1;
	while(IS_UPPER(c)||IS_LOWER(c)||c=='\\'){
		if(first){
			if(c==left){
				printf("%c%c%c\t%c==%c\n",yytext[0],yytext[1],yytext[2],c,left);
				leftRecursivity();
			}
			first=0;
		}
		char * p = malloc(sizeof(c));
		*p=c;
		insert(right, p);
		c=yytext[i++];
	}
	addProduction();
}

<P2_STATE>(,[\ \t\r\n]*) {
	BEGIN P_STATE;
}

<P2_STATE>(\}[\ \t\r\n]*) {
	BEGIN END_STATE;
}

<END_STATE>(\)) {
	fin();
};


%%

int isSpace(char c){
	return (c==' ' || c=='\t' || c=='\n');
}

void setName(char * string){
	int n, i;
	for(n=0; string[n]!=' '&&string[n]!='='&&string[n]!='\n';n++);
	char* name=malloc(n);
	for(i=0; i<n; i++){
		name[i]=string[i];
	}
	name[i]=0;
	G->name=name;
}

void addProduction(){
	production *p=malloc(sizeof(production));
	p->left=left;
	p->right=right;
	insert(G->P,p);
	right=newList(compareC);
}

void fin(){
	//printf("fin\n");
}

void addVN(char c){
	G->VN[c-'A']=1;
}

void addVT(char c){
	G->VT[c-'a']=1;
}

void setS(char c){
	G->S=c;
}

void
invalidGrammar()
{
	printf("\nGramática Inválida\n");
	exit(1);
}

void
leftRecursivity()
{
	printf("\nLa gramática no debe tener recursividad izquierda\n");
	exit(1);
}

int grwrap(void){
	return 1;
}
