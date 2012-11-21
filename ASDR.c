#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

struct production {
	char *data;
	struct production *next;
};

static bool process  ( char *str, struct production prod );
static bool process_A( char *str, struct production prod );
static bool process_B( char *str, struct production prod );
static bool process_C( char *str, struct production prod );

static bool (*non_terminal_function[0x100]) ( char*, struct production ) = {
	['A'] = process_A,
	['B'] = process_B,
	['C'] = process_C,
};

static int productions_count[ 3 ] = {0};

static const char *productions[] = {
	"A->aBC",
	"A->cB",
	"B->aA",
	"B->b",
	"C->c",
	"C->\\",
};


static bool process( char *str, struct production prod ) {

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
		return process( str, *prod.next );
	}

	return !*str;
}


bool process_A( char *str, struct production prod ) {

	if ( process( str, (struct production){ "aBC", &prod } ) ) {
		productions_count[ 0 ]++;
		return true;
	}

	if ( process( str, (struct production){ "cB", &prod } ) ) {
		productions_count[ 1 ]++;
		return true;
	}

	return false;
}


bool process_B( char *str, struct production prod ) {

	if ( process( str, (struct production){ "aA", &prod } ) ) {
		productions_count[ 2 ]++;
		return true;
	}

	if ( process( str, (struct production){ "b", &prod } ) ) {
		productions_count[ 3 ]++;
		return true;
	}

	return false;
}


bool process_C( char *str, struct production prod ) {

	if ( process( str, (struct production){ "c", &prod } ) ) {
		productions_count[ 4 ]++;
		return true;
	}

	if ( process( str, (struct production){ "\\", &prod } ) ) {
		productions_count[ 5 ]++;
		return true;
	}

	return false;
}


int main( int argc, char **argv ) {

	int i;

	if ( argc != 2 ) {

		printf( "Usage:\n\tASDR word\n" );
		return 1;
	}

	if ( process_A( argv[1], (struct production){ "", NULL } ) ) {

		printf( "%s belongs.\n", argv[1] );
		printf( "The productions used were:\n" );

		for ( i = 0; i < 6; i++ ) {
			if ( productions_count[ i ] ) {
				printf( "\t%s\n", productions[ i ] );
			}
		}

	} else {

		printf( "%s doesn't belong.\n", argv[1] );
	}

	return 0;
}
