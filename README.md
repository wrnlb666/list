# list/slice/array_list/vector
list in pure C with runtime generic


## Example for quick start
```c
#include "src/list.h"

int main( void )
{
    // list creation
    int* arr = list_create( (list_args_t) { .type = LIST_I32 } );
    // or you can do
    // int* arr = list_create_args( LIST_I32 );

    // append in a for loop
    for ( int i = 0; i < 50; i++ )
    {
        arr = list_append( arr, i );
    }

    // how to access list value? exactly like how you would access a normal array!
    for ( size_t i = 0; i < list_len(arr); i++ )
    {
        printf( "%3d", arr[i] );
    }
    printf( "\n" );

    // list clean up
    list_destroy(arr);

    return 0;
}
```
