#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/list.h"


// a very basic size string struct
typedef struct str
{
    size_t length;
    char* string;
} str_t;


// copy constructor of str_t struct
void str_copy( void* dest, const void* restrict src )
{
    size_t size = strlen( (char*) src );
    ( (str_t*) dest )->length = size;
    ( (str_t*) dest )->string = malloc( size + 1 );
    strcpy( ( (str_t*) dest )->string, (char*) src );
    return;
}

// destructor of str_t struct
void str_free( void* str )
{
    free( ( (str_t*) str )->string );
}


int main( void )
{
    str_t* arr = list_create_args( LIST_STRUCT, sizeof (str_t), .attr = { .copy = str_copy, .free = str_free } );

    char *arr1[] = { "Hello", " ", "World", "!", "\n" };
    size_t array_size = sizeof (arr1) / sizeof (char*);

    // append array to list
    for ( size_t i = 0; i < 2; i++ )
    {
        arr = list_append( arr, arr1[i] );
    }
    for ( size_t i = 2; i < array_size; i++ )
    {
        arr = list_insert( arr, i, arr1[i] );
    }

    // display initial list
    for ( size_t i = 0; i < list_len(arr); i++ )
    {
        printf( "%s", arr[i].string );
    }

    // pop two elements from the list
    arr = list_pops( arr, 1, 2 );

    // display the changed list
    for ( size_t i = 0; i < list_len(arr); i++ )
    {
        printf( "%s", arr[i].string );
    }

    // clean up
    list_destroy( arr );

    return 0;
}

