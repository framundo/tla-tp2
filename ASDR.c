#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_SIZE 1024

struct production {
	char *data;
	struct production *next;
};

static bool procesar  ( char *str, struct production prod );
static bool procesar_A( char *str, struct production prod );
static bool procesar_B( char *str, struct production prod );

static bool (*non_terminal_function[0x100]) ( char*, struct production ) = {
	['A'] = procesar_A,
	['B'] = procesar_B,
};

static int productions_count[ 4 ] = {0};

static const char *productions[] = {
	"A->aB",
	"A->c",
	"B->aA",
	"B->b",
};


static int used[MAX_SIZE]={0};
int indexP=0;

void add_production(int i){
	used[indexP++]=i;
}


static bool procesar( char *str, struct production prod ) {

	while ( *prod.data ) {

		char c = *prod.data++;

		if ( isupper( c ) ) {
			return non_terminal_function[ c ]( str, prod );
		}

		if ( c == '\\' ) {
			continue;
		}

		if ( *str++ != c ) {
			return false;
		}
	}

	if ( prod.next ) {
		return procesar( str, *prod.next );
	}

	return !*str;
}


bool procesar_A( char *str, struct production prod ) {

	if ( procesar( str, (struct production){ "aB", &prod } ) ) {
		add_production(0);
		productions_count[ 0 ]++;
		return true;
	}

	if ( procesar( str, (struct production){ "c", &prod } ) ) {
		add_production(1);
		productions_count[ 1 ]++;
		return true;
	}

	return false;
}


bool procesar_B( char *str, struct production prod ) {

	if ( procesar( str, (struct production){ "aA", &prod } ) ) {
		add_production(2);
		productions_count[ 2 ]++;
		return true;
	}

	if ( procesar( str, (struct production){ "b", &prod } ) ) {
		add_production(3);
		productions_count[ 3 ]++;
		return true;
	}

	return false;
}


int main( int argc, char **argv ) {

	int i;

	if ( argc != 2 ) {

		printf( "Uso:\n\tASDR cadena\n" );
		return 1;
	}

	if ( procesar_A( argv[1], (struct production){ "", NULL } ) ) {

		printf( "%s pertenece.\n", argv[1] );
		printf( "El conjunto de producciones usadas es:\n" );

		for ( i = 0; i < 4; i++ ) {
			if ( productions_count[ i ] ) {
				printf( "\t%s\n", productions[ i ] );
			}
		}
		printf("En el siguiente orden:\n");
		for(i=indexP-1; i>=0; i--){
			printf("%s	",productions[used[i]]);
		}
		printf("\n");

	} else {

		printf( "%s no pertenece.\n", argv[1] );
	}

	return 0;
}
