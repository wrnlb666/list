// example for sturct
#include "src/list.h"
#include <stdint.h>

typedef struct test
{
    int64_t x;
    double  y;
} test_t;

int main( void )
{
    // create list
    test_t* arr = list_create_args( LIST_STRUCT, .size = sizeof (test_t) );

    // append each element to the end to list
    for ( size_t i = 0; i < 100; i++ )
    {
        // NOTE: for `LIST_STRUCT`, you need to pass by reference
        arr = list_append( arr, & (test_t) { .x = i, .y = i } );
    }

    // grow the list to size of 200, and assign value manually
    arr = list_grow( arr, 200 );
    for ( size_t i = 100; i < 200; i++ )
    {
        arr[i] = (test_t) { .x = i + 100, .y = i + 100 };
    }

    // insert an array into specific index of the list
    test_t* temp = malloc( sizeof (test_t) * 100 );
    for ( size_t i = 0; i < 100; i++ )
    {
        temp[i] = (test_t) { .x = i + 100, .y = i + 100 };
    }
    arr = list_inserts( arr, 100, 100, temp );
    free(temp);

    // get the length of the list and print the list
    size_t length = list_len(arr);
    printf( "list length = %zu\n", length );
    for ( size_t i = 0; i < length; i++ )
    {
        printf( "%ld, %lf\n", arr[i].x, arr[i].y );
    }
    
    // clean up
    list_destroy(arr);

    return 0;
}
