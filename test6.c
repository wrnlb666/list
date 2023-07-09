#include "src/list.h"

typedef struct
{
    char str[128];
} str_t;

void str_copy( void* dest, const void* src )
{
    str_t* d = (str_t*) dest;
    strncpy( d->str, src, 127 );
    d->str[127] = 0;
}

int main( int argc, char** argv )
{
    list_args_t list_args = 
    {
        .type = LIST_STRUCT,
        .size = sizeof (str_t),
        .attr = 
        {
            .copy = str_copy,
        }
    };
    FILE* fp1 = fopen( "test6.bin", "rb" );
    if ( fp1 == NULL )
    {
        goto serialize;
    }
    str_t* arr1 = list_deserialize( list_args, fp1 );
    fclose( fp1 );
    if ( arr1 == NULL )
    {
        fprintf( stderr, "Fail to deserialize.\n" );
        exit(1);
    }
    size_t size = list_len(arr1);
    printf( "args: [ " );
    for ( size_t i = 0; i < size; i++ )
    {
        printf( "%s ", arr1[i].str );
    }
    printf( "]\n" );
    list_destroy( arr1 );

serialize:
    (void) argc;
    str_t* arr2 = list_create( list_args );
    for ( int i = 0; i < argc; i++ )
    {
        arr2 = list_append( arr2, argv[i] );
    }
    FILE* fp2 = fopen( "test6.bin", "wb+" );
    if ( !list_serialize( arr2, fp2 ) )
    {
        fprintf( stderr, "Fail to serialize.\n" );
        exit(1);
    }
    fclose( fp2 );
    list_destroy( arr2 );

    return 0;
}