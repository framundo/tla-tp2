#include "grammar.h"

int VNToNumber(grammar* G, char c){
	if(G->S==c){
		return 0;
	}
	return c-'A'+1;
}

/*char findFinal(grammar* G){
	production* p;
	while((p=(production*)getNext(G->P))!=NULL){
		if(p->right1=='\\'){
			return p->left;
		}
	}
	return 0;
}*/

/*void toAFD(grammar* G){
	char* filename = malloc(strlen(G->name)+4);
	sprintf(filename, "%s.dot", G->name);
	FILE * dot = fopen(filename, "w");
	fprintf(dot, "digraph{\n");
	int i,j=0;
	G->VN[G->S-'A']=0;
	fprintf(dot, "node [shape=circle] Node0 [label=\"0\"];\n");
	production* p;
	reset(G->P);
	char final=0;
	final=findFinal(G);
	for(j=0; j<SIZE; j++){
		int vn = G->VN[j];
		if(vn){
			int n=(j==G->S-'A')?0:j+1;
			if(j!=final-'A'){
				fprintf(dot, "node [shape=circle] Node%d [label=\"%d\"];\n", n, n);
			}else{
				fprintf(dot, "node [shape=doublecircle] Node%d [label=\"%d\"];\n", n,n);
			}
		}
	}
	reset(G->P);
	while((p=(production*)getNext(G->P))!=NULL){
		i=VNToNumber(G, p->left);
		if(p->right2==0 && p->right1!='\\'){
			fprintf(dot, "Node%d -> Node%d [label=\"\\\\\"];\n", i, VNToNumber(G,p->right1));
		}else if(p->right1!='\\'){
			fprintf(dot, "Node%d -> Node%d [label=\"%c\"];\n", i, VNToNumber(G,p->right2), p->right1);
		}
	}

	fprintf(dot, "}");
	fclose(dot);
}*/

int compareP(void* p1, void* p2){
	production * pr1=(production*)p1;
	production * pr2=(production*)p2;
	int result=(pr1->left - pr2->left)*10;
	int multiplier=100;
	char *c1,*c2;
	reset(pr1->right);
	while((c1=getNext(pr1->right))!=NULL)
	{
		result+=(*c1)*multiplier;
		multiplier*=100;
	};
	multiplier=100;
	reset(pr2->right);
	while((c2=getNext(pr2->right))!=NULL)
	{
		result-=(*c2)*multiplier;
		multiplier*=100;
	};
	printf("%d\n",result);
	return result;
}

int compareC(void* c1, void* c2){
	char s1=*((char*)c1);
	char s2=*((char*)c2);
	return s1-s2;
}

grammar* newGrammar(){
	grammar* G=malloc(sizeof(grammar));
	int i;
	for(i=0; i<SIZE; i++){
		G->VN[i]=0;
		G->VT[i]=0;
	}
	G->VN[i]=0;
	G->P=newList(compareP);
	return G;
}

void printProduction(production* p){
	printf("%c->", p->left);
	char* c;
	reset(p->right);
	while((c=getNext(p->right))!=NULL){
		printf("%c", *c);
	}
}

void printGrammar(grammar * g){
	printf("%s = ( {", g->name);
	int i, first=1;
	for(i=0; i<SIZE; i++){
		if(g->VN[i]){
			if(!first){
				printf(",");
			}else{
				first=0;
			}
			printf(" %c", i+'A');
		}
	}
	printf(" }, {");
	first=1;
	for(i=0; i<SIZE; i++){
		if(g->VT[i]){
			if(!first){
				printf(",");
			}else{
				first=0;
			}
			printf(" %c", i+'a');
		}
	}
	printf("} , %c, {",g->S);
	reset(g->P);
	first=1;
	production * aProduction;
	while((aProduction=(production*)getNext(g->P))){
		if(!first){
			printf(",");
		}else{
			first=0;
		}
		printProduction(aProduction);
	}
	printf("} )\n");
}

/*int main(){
	grammar G;
	int i;
	for(i=3; i<26; i++){
		G.VN[i]=0;
		G.VT[i]=0;	
	}
	for(i=0; i<3; i++){
		G.VN[i]=1;
		G.VT[i]=1;
	}
	G.S='A';
	G.name="fedeGrammar";
	G.P = newList(compareP);

	production* p=malloc(sizeof(production));
	p->left='A';
	p->right1='a';
	p->right2='B';
	insert(G.P, p);
	p=malloc(sizeof(production));
	p->left='B';
	p->right1='a';
	p->right2='A';
	insert(G.P, p);
	p=malloc(sizeof(production));
	p->left='B';
	p->right1='b';
	p->right2='@';
	insert(G.P, p);
	p=malloc(sizeof(production));
	p->left='@';
	p->right1='\\';
	insert(G.P, p);

	toAFD(&G);
	return 0;
}*/
