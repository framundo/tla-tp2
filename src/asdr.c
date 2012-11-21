#include "asdr.h"

static void print_declarations( FILE *file, struct grammar *grammar ) {
	int i;
    fprintf( file, "#include <stdio.h>\n" );
    fprintf( file, "#include <stdlib.h>\n" );
    fprintf( file, "#include <string.h>\n" );
    fprintf( file, "#include <ctype.h>\n" );
    fprintf( file, "#include <stdbool.h>\n" );
    fprintf( file, "\n" );
    fprintf( file, "struct production {\n" );
    fprintf( file, "\tchar *data;\n" );
    fprintf( file, "\tstruct production *next;\n" );
    fprintf( file, "};\n" );
    fprintf( file, "\n" );

    fprintf( file, "static bool process  ( char *str, struct production prod );\n");

    for ( i = 0; i < SIZE; i++ ) {
		if(grammar->VN[i]){
			fprintf( file, "static bool process_%c( char *str, struct production prod );\n", i+'A' );
        }
    }

    fprintf( file, "\n" );
    fprintf( file, "static bool (*non_terminal_function[0x100]) ( char*, struct production ) = {\n" );

	int size=0;
    for ( i = 0; i < SIZE; i++ ) {
		if(grammar->VN[i]){
			size++;
			fprintf( file, "\t['%c'] = process_%c,\n", i+'A', i+'A' );
		}
    }

    fprintf( file, "};\n" );
    fprintf( file, "\n" );
    fprintf( file, "static int productions_count[ %d ] = {0};\n", size );
    fprintf( file, "\n" );
    fprintf( file, "static const char *productions[] = {\n" );
    
    production *p;
    reset(grammar->P);
    while(p=getNext(grammar->P)){
		reset(p->right);
		char *c=getNext(p->right);
		if ( *c != '\\' ) {
					fprintf( file, "\t\"%c->%c", p->left,*c);
					while(c=getNext(p->right)){
						fprintf( file, "%c", *c);
					}
					fprintf(file, "\",\n");
				} else {
					fprintf( file, "\t\"%c->\\\\\",\n", p->left );
				}
	}

    fprintf( file, "};\n" );
}


static void print_process_function( FILE *file, struct grammar *grammar ) {

    fprintf( file, "\n" );
    fprintf( file, "\n" );
    fprintf( file, "static bool process( char *str, struct production prod ) {\n");
    fprintf( file, "\n" );
    fprintf( file, "\twhile ( *prod.data ) {\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tchar c = *prod.data++;\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tif ( isupper( c ) ) {\n" );
    fprintf( file, "\t\t\treturn non_terminal_function[ c ]( str, prod );\n" );
    fprintf( file, "\t\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tif ( c == '\\\\' ) {\n" );
    fprintf( file, "\t\t\tcontinue;\n" );
    fprintf( file, "\t\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tif ( *str++ != c ) {\n" );
    fprintf( file, "\t\t\treturn false;\n" );
    fprintf( file, "\t\t}\n" );
    fprintf( file, "\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\tif ( prod.next ) {\n" );
    fprintf( file, "\t\treturn process( str, *prod.next );\n" );
    fprintf( file, "\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\treturn !*str;\n" );
    fprintf( file, "}\n" );
}


static void print_non_terminal_function( FILE *file, grammar *grammar, char vn , int* pos) {

    production* p;

    fprintf( file, "\n" );
    fprintf( file, "\n" );
    fprintf( file, "bool process_%c( char *str, struct production prod ) {\n", vn );
    fprintf( file, "\n" );

	reset(grammar->P);
	int size=0;
    while(p=getNext(grammar->P)){
		if(p->left==vn){
			size++;
			reset(p->right);
			char *c=getNext(p->right);
			if ( *c != '\\' ) {
				fprintf( file, "\tif ( process( str, (struct production){ \"%c", *c);
				while(c=getNext(p->right)){
					fprintf( file, "%c", *c);
				}
				fprintf( file, "\", &prod } ) ) {\n");
			} else {
				fprintf( file, "\tif ( process( str, (struct production){ \"\\\\\", &prod } ) ) {\n" );
			}

			fprintf( file, "\t\tproductions_count[ %d ]++;\n", (*pos)++ );
			fprintf( file, "\t\treturn true;\n" );
			fprintf( file, "\t}\n" );
			fprintf( file, "\n" );
		}
    }

    fprintf( file, "\treturn false;\n" );
    fprintf( file, "}\n" );
}


static void print_main( FILE *file, struct grammar *grammar ) {

    fprintf( file, "\n" );
    fprintf( file, "\n" );
    fprintf( file, "int main( int argc, char **argv ) {\n" );
    fprintf( file, "\n" );
    fprintf( file, "\tint i;\n" );
    fprintf( file, "\n" );
    fprintf( file, "\tif ( argc != 2 ) {\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tprintf( \"Usage:\\n\\tASDR word\\n\" );\n" );
    fprintf( file, "\t\treturn 1;\n" );
    fprintf( file, "\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\tif ( process_%c( argv[1], (struct production){ \"\", NULL } ) ) {\n", grammar->S );
    fprintf( file, "\n" );
    fprintf( file, "\t\tprintf( \"%%s belongs.\\n\", argv[1] );\n" );
    fprintf( file, "\t\tprintf( \"The productions used were:\\n\" );\n" );
    fprintf( file, "\n" );
    int size=0;
    reset(grammar->P);
    production *p;
    while(p=getNext(grammar->P)){
		size++;
	}
    fprintf( file, "\t\tfor ( i = 0; i < %d; i++ ) {\n", size );
    fprintf( file, "\t\t\tif ( productions_count[ i ] ) {\n" );
    fprintf( file, "\t\t\t\tprintf( \"\\t%%s\\n\", productions[ i ] );\n" );
    fprintf( file, "\t\t\t}\n" );
    fprintf( file, "\t\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t} else {\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tprintf( \"%%s doesn't belong.\\n\", argv[1] );\n" );
    fprintf( file, "\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\treturn 0;\n" );
    fprintf( file, "}\n" );
}


void print_parser( FILE *file, grammar *g ) {

    print_declarations( file, g );

    print_process_function( file, g );
	int i, j=0;
    for ( i = 0;
     i<SIZE; i++ ) {
		if(g->VN[i]){
			print_non_terminal_function( file, g, i+'A', &j );
		}
    }

    print_main( file, g );
}

void generateASDR(grammar* g){
	FILE* source;
	source=fopen("ASDR.c","w");
	print_parser(source,g);
	fclose(source);
}

