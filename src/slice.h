#ifndef __SLICE_H__
#define __SLICE_H__

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


typedef enum
{
    SLICE_CHAR,     // char
    SLICE_WCHAR,    // wchar_t
    SLICE_I32,      // int32_t
    SLICE_U32,      // uint32_t
    SLICE_F32,      // float
    SLICE_I64,      // int64_t
    SLICE_U64,      // uint64_t
    SLICE_F64,      // double
    SLICE_PTR,      // void*
    SLICE_STR,      // char*
    SLICE_GENERIC,  // struct
} slice_type_t;

typedef void (*slice_copy)( void** dest, const void* restrict src );    // if not specified, memcpy will be performed for SLICE_GENERIC, shallow copy for all others
typedef void (*slice_free)( void** ptr );                               // ptr will be the address of that element, so void** for SLICE_PTR

typedef struct
{
    slice_type_t    type;   // type of the elements
    size_t          size;   // size of a single element
    slice_copy      copy;   // copy constructor for custom element type
    slice_free      free;   // desctructor for custom element type
} slice_args_t;



// functions
void*   slice_new( slice_args_t args );
void    slice_delete( void* slice );
size_t  slice_size( void* slice );                                      // get the current size of the slice. 
void*   slice_resize( void* slice, size_t size );                       // Return new address if the array. You need to make sure data is valid if `slice_free` is set. 
void*   slice_grow( void* slice, size_t size );                         // Return new address of the array. Do nothing if size is not bigger than current size. 
void*   slice_shrink( void* slice, size_t size );                       // Return new address of the array. Do nothing if size is not smaller than current size. 
void*   slice_append( void* slice, /* T value */ ... );                 // with SLICE_GENERIC, pass in the address of the struct. Return new address of the array. 
void*   slice_insert( void* slice, size_t index, size_t size, ... );    // Return new address of the array. 
void*   slice_pop( void* slice, size_t index );                         // Return new address of the array. 





// macro
#define slice_new_args( ... )               slice_new( (slice_args_t) { __VA_ARGS__ } );    // slice_new( slice_type_t type, size_t size, slice_copy copy, slice_free free );
        



#endif  // __SLICE_H__