// Example with custom allocator also pointer type. 
// half generic? `void*` is for sure generic, but it is not as convinent

#define ARENA_IMPLEMENTATION
#include "arena.h"
#include "src/list.h"
#include <inttypes.h>


typedef struct
{
    int64_t x;
    double  y;
} test_t;

static arena_t data_arena = { 0 };
void copy( void** dest, const void* src )
{
    test_t* d = arena_alloc( &data_arena, sizeof (test_t) );
    // test_t* d = malloc( sizeof (test_t) );
    d->x = ( (test_t*) src )->x;
    d->y = ( (test_t*) src )->y;
    *dest = d;
    return;
}

void test_free( void** ptr )
{
    free( *ptr );
    ptr = NULL;
}

static arena_t list_arena = { 0 };
void* list_allocate( size_t size )
{
    return arena_alloc( &list_arena, size );
}
void* list_reallocate( void* ptr, size_t old_size, size_t new_size )
{
    return arena_realloc( &list_arena, ptr, old_size, new_size );
}

int main( void )
{
    test_t** arr = list_create_args( LIST_PTR, .attr = { .copy = copy }, .alloc = { list_allocate, list_reallocate, NULL } );

    for ( size_t i = 0; i < 2000; i++ )
    {
        arr = list_append( arr, & (test_t) { .x = (int64_t) i, .y = (double) i } );
    }

    printf( "list_size: %zu\n", list_len(arr) );

    for ( size_t i = 0; i < list_len(arr); i++ )
    {
        printf( "%3ld, %3.1lf\n", arr[i]->x, arr[i]->y );
    }

    list_destroy( arr );

    arena_free( &list_arena );
    arena_free( &data_arena );

    return 0;
}

