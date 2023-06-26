#include "src/slice.h"
#include <inttypes.h>


typedef struct
{
    int64_t x;
    double  y;
} test_t;


static void copy( void** dest, const void* src )
{
    *dest = malloc( sizeof (int) );
    int* d = *(int**) dest;
    const int* s = src;
    *d = *s;
}

static void s_free( void** ptr )
{
    free( *ptr );
    *ptr = NULL;
}


int main( void )
{
    int* arr = slice_new_args( .type = SLICE_I32 );

    int arr1[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    for ( int i = 0; i < 36; i++ )
    {
        arr = slice_append( arr, i );
    }

    arr = slice_insert( arr, 20, 10, arr1 );

    for ( size_t i = 0; i < slice_size(arr); i++ )
    {
        printf( "%3d", arr[i] );
    }
    printf( "\n" );

    slice_delete( arr );

    return 0;
}