#include "src/list.h"

int main( void )
{
    list_args_t list_args = 
    {
        .type = LIST_I32,
    };
    int32_t* arr1 = list_create( list_args );

    for ( int32_t i = 0; i < 30; i++ )
    {
        arr1 = list_append( arr1, i );
    }

    FILE* fp1 = fopen( "test5.bin", "wb+" );
    if ( !list_serialize( arr1, fp1 ) )
    {
        fprintf( stderr, "Fail to serialize.\n" );
        exit(1);
    }
    fclose( fp1 );
    list_destroy( arr1 );

    FILE* fp2 = fopen( "test5.bin", "rb" );
    int32_t* arr2 = list_deserialize( list_args, fp2 );
    if ( arr2 == NULL )
    {
        fprintf( stderr, "Fail to deserialize.\n" );
        exit(1);
    }
    fclose( fp2 );

    for ( size_t i = 0; i < list_len(arr2); i++ )
    {
        printf( "%3d", arr2[i] );
    }
    printf( "\n" );
    list_destroy( arr2 );

    return 0;
}