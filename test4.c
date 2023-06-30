#include "src/list.h"

int main( void )
{
    char* s1 = "Hello";
    char* s2 = " ";
    char* s3 = "World";
    char* s4 = "!";
    char* s5 = "\n";

    char *s[] = { s3, s2, s1, s4, s5 };

    char** arr = list_create_args( .type = LIST_STR );

    arr = list_append( arr, s1 );
    arr = list_append( arr, s2 );
    arr = list_append( arr, s3 );
    arr = list_append( arr, s4 );
    arr = list_append( arr, s5 );
    arr = list_inserts( arr, 5, 5, s );

    arr = list_pop( arr, 4 );

    for ( size_t i = 0; i < list_len( arr ); i++ )
    {
        printf( "%s", arr[i] );
    }

    list_destroy( arr );

    return 0;
}