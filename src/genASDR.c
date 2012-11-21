#include "grammar.h"
#include <string.h>
#include <unistd.h>
#include "asdr.h"

#define YY_BUF_SIZE 16384

grammar* G;

int main(int argc, char **argv){
	//argc=2; 
	//argv[1]=malloc(20);
	//strcpy(argv[1], "./archivos gr/gr1.gr");
	if ( argc!=2 ) {
        printf( "Uso:\ngenASDR entrada\n" );
        return 1;
    }
    FILE* file = fopen( argv[1], "r" );
    if ( !file ) {
        printf( "Error abriendo archivo\n" );
        return 1;
    }
    int len = strlen(argv[1]);
    if(strcmp(argv[1]+len-3,".gr")==0){
		printf("Leyendo archivo .gr\n");
		G=newGrammar();
		gr_switch_to_buffer( gr_create_buffer( file, YY_BUF_SIZE ) );
		if(!grlex()){
			printGrammar(G);
			//TODO
			generateASDR(G);
            printf("\n");
		}
		return 1;
	}
	else{
		printf("El archivo debe tener extension .gr\n");
		return 1;
	}
}
