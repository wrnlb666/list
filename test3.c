// basic example for primitive type

#include "src/list.h"

int main( void )
{
    // list creation
    int* arr = list_new( (list_args_t) { .type = LIST_I32 } );
    // or you can do
    // int* arr = list_new_args( LIST_I32 );

    // append in a for loop
    for ( int i = 0; i < 30; i++ )
    {
        arr = list_append( arr, i );
    }

    // how to access list value? exactly like how you would access a normal array!
    for ( int i = 0; i < 30; i++ )
    {
        printf( "%3d", arr[i] );
    }
    printf( "\n" );

    // list clean up
    list_delete(arr);

    return 0;
}