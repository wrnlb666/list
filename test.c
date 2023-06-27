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
    d->x = ( (test_t*) src )->x;
    d->y = ( (test_t*) src )->y;
    *dest = d;
    return;
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
    test_t** arr = list_new_args( LIST_PTR, .copy = copy, .alloc = (list_alloc_t) { list_allocate, list_reallocate, NULL } );

    for ( int i = 0; i < 2060; i++ )
    {
        arr = list_append( arr, & (test_t) { .x = (int64_t) i, .y = (double) i } );
    }

    printf( "list_size: %zu\n", list_len(arr) );

    for ( size_t i = 0; i < list_len(arr); i++ )
    {
        printf( "%3ld, %3.1lf\n", (*arr)[i].x, (*arr)[i].y );
    }
    printf( "\n" );

    list_delete( arr );

    arena_free( &list_arena );
    arena_free( &data_arena );

    return 0;
}

