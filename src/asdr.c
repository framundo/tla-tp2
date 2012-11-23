#include "asdr.h"

static void declare( FILE *file, grammar *g ) {
	int i;
    fprintf( file, "#include <stdio.h>\n" );
    fprintf( file, "#include <stdlib.h>\n" );
    fprintf( file, "#include <string.h>\n" );
    fprintf( file, "#include <ctype.h>\n" );
    fprintf( file, "#include <stdbool.h>\n" );
    fprintf( file, "\n" );
    fprintf( file, "#define MAX_SIZE 1024\n\n");
    fprintf( file, "struct production {\n" );
    fprintf( file, "\tchar *data;\n" );
    fprintf( file, "\tstruct production *next;\n" );
    fprintf( file, "};\n" );
    fprintf( file, "\n" );

    fprintf( file, "static bool procesar  ( char *str, struct production prod );\n");

    for ( i = 0; i < SIZE; i++ ) {
		if(g->VN[i]){
			fprintf( file, "static bool procesar_%c( char *str, struct production prod );\n", i+'A' );
        }
    }

    fprintf( file, "\n" );
    fprintf( file, "static bool (*non_terminal_function[0x100]) ( char*, struct production ) = {\n" );

    for ( i = 0; i < SIZE; i++ ) {
		if(g->VN[i]){
			fprintf( file, "\t['%c'] = procesar_%c,\n", i+'A', i+'A' );
		}
    }

	int size=0;
    reset(g->P);
    production *p;
    while(p=getNext(g->P)){
		size++;
	}

    fprintf( file, "};\n" );
    fprintf( file, "\n" );
    fprintf( file, "static int productions_count[ %d ] = {0};\n", size );
    fprintf( file, "\n" );
    fprintf( file, "static const char *productions[] = {\n" );
    
    for(i=0; i<SIZE; i++){
		reset(g->P);
		if(g->VN[i]){
			while(p=getNext(g->P)){
				if(p->left == i+'A'){
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
			}
		}
	}
	
	
    fprintf( file, "};\n\n" );
    fprintf( file,"\nstatic int used[MAX_SIZE]={0};\n");
	fprintf( file,"int indexP=0;\n");
	
	fprintf(file,"\nvoid add_production(int i){\n");
	fprintf(file,"\tused[indexP++]=i;\n");
	fprintf(file,"}\n");
}


static void define( FILE *file, grammar *g ) {

    fprintf( file, "\n" );
    fprintf( file, "\n" );
    fprintf( file, "static bool procesar( char *str, struct production prod ) {\n");
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
    fprintf( file, "\t\treturn procesar( str, *prod.next );\n" );
    fprintf( file, "\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\treturn !*str;\n" );
    fprintf( file, "}\n" );
}


static void define_non_terminal_function( FILE *file, grammar *g, char vn , int* pos) {

    production* p;

    fprintf( file, "\n" );
    fprintf( file, "\n" );
    fprintf( file, "bool procesar_%c( char *str, struct production prod ) {\n", vn );
    fprintf( file, "\n" );

	reset(g->P);
	int size=0;
    while(p=getNext(g->P)){
		if(p->left==vn){
			size++;
			reset(p->right);
			char *c=getNext(p->right);
			if ( *c != '\\' ) {
				fprintf( file, "\tif ( procesar( str, (struct production){ \"%c", *c);
				while(c=getNext(p->right)){
					fprintf( file, "%c", *c);
				}
				fprintf( file, "\", &prod } ) ) {\n");
			} else {
				fprintf( file, "\tif ( procesar( str, (struct production){ \"\\\\\", &prod } ) ) {\n" );
			}
			fprintf( file, "\t\tadd_production(%d);\n", *pos);
			fprintf( file, "\t\tproductions_count[ %d ]++;\n", (*pos)++ );
			fprintf( file, "\t\treturn true;\n" );
			fprintf( file, "\t}\n" );
			fprintf( file, "\n" );
		}
    }

    fprintf( file, "\treturn false;\n" );
    fprintf( file, "}\n" );
}


static void define_main( FILE *file, grammar *g ) {

    fprintf( file, "\n" );
    fprintf( file, "\n" );
    fprintf( file, "int main( int argc, char **argv ) {\n" );
    fprintf( file, "\n" );
    fprintf( file, "\tint i;\n" );
    fprintf( file, "\n" );
    fprintf( file, "\tif ( argc != 2 ) {\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tprintf( \"Uso:\\n\\tASDR cadena\\n\" );\n" );
    fprintf( file, "\t\treturn 1;\n" );
    fprintf( file, "\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\tif ( procesar_%c( argv[1], (struct production){ \"\", NULL } ) ) {\n", g->S );
    fprintf( file, "\n" );
    fprintf( file, "\t\tprintf( \"%%s pertenece.\\n\", argv[1] );\n" );
    fprintf( file, "\t\tprintf( \"El conjunto de producciones usadas es:\\n\" );\n" );
    fprintf( file, "\n" );
    int size=0;
    reset(g->P);
    production *p;
    while(p=getNext(g->P)){
		size++;
	}
    fprintf( file, "\t\tfor ( i = 0; i < %d; i++ ) {\n", size );
    fprintf( file, "\t\t\tif ( productions_count[ i ] ) {\n" );
    fprintf( file, "\t\t\t\tprintf( \"\\t%%s\\n\", productions[ i ] );\n" );
    fprintf( file, "\t\t\t}\n" );
    fprintf( file, "\t\t}\n" );
    fprintf( file, "\t\tprintf(\"En el siguiente orden:\\n\");\n");
    fprintf( file, "\t\tfor(i=indexP-1; i>=0; i--){\n");
	fprintf( file, "\t\t\tprintf(\"%%s\t\",productions[used[i]]);\n");
	fprintf( file, "\t\t}\n");
	fprintf( file, "\t\tprintf(\"\\n\");\n");
    fprintf( file, "\n" );
    fprintf( file, "\t} else {\n" );
    fprintf( file, "\n" );
    fprintf( file, "\t\tprintf( \"%%s no pertenece.\\n\", argv[1] );\n" );
    fprintf( file, "\t}\n" );
    fprintf( file, "\n" );
    fprintf( file, "\treturn 0;\n" );
    fprintf( file, "}\n" );
}


void print_all( FILE *file, grammar *g ) {

    declare( file, g );

    define( file, g );
	int i, j=0;
    for ( i = 0; i<SIZE; i++ ) {
		if(g->VN[i]){
			define_non_terminal_function( file, g, i+'A', &j );
		}
    }

    define_main( file, g );
}

void generateASDR(grammar* g){
	FILE* source;
	source=fopen("ASDR.c","w");
	print_all(source,g);
	fclose(source);
}

