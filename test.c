#define ARENA_IMPLEMENTATION
#include "arena.h"
#include "src/slice.h"
#include <inttypes.h>


typedef struct
{
    int64_t x;
    double  y;
} test_t;


// if store by dynamically stored pointers. But I don't see any reason
static Arena data_arena = { 0 };
static void copy( void** dest, const void* src )
{
    *dest = arena_alloc( &data_arena, sizeof (test_t) );
    test_t* d = *(test_t**) dest;
    const test_t* s = src;
    d->x = s->x;
    d->y = s->y;
    return;
}
static void s_free( void** ptr )
{
    (void) ptr;
    return;
}

static Arena slice_arena = { 0 };
void* slice_allocate( size_t size )
{
    return arena_alloc( &slice_arena, size );
}
void* slice_reallocate( void* ptr, size_t old_size, size_t new_size )
{
    return arena_realloc( &slice_arena, ptr, old_size, new_size );
}

int main( void )
{
    test_t* arr = slice_new_args( SLICE_STRUCT, sizeof (test_t), .allocator = (slice_alloc_t) { slice_allocate, slice_reallocate, NULL } );

    for ( int i = 0; i < 36; i++ )
    {
        arr = slice_append( arr, & (test_t) { .x = (int64_t) i, .y = (double) i } );
    }

    printf( "slice_size: %zu\n", slice_size(arr) );

    for ( size_t i = 0; i < slice_size(arr); i++ )
    {
        printf( "%3ld, %3.1lf\n", arr[i].x, arr[i].y );
    }
    printf( "\n" );

    slice_delete( arr );

    arena_free( &slice_arena );
    arena_free( &data_arena );

    return 0;
}