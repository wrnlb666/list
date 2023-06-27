#ifndef __LIST_H__
#define __LIST_H__

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
    LIST_CHAR,     // char
    LIST_WCHAR,    // wchar_t
    LIST_I32,      // int32_t
    LIST_U32,      // uint32_t
    LIST_F32,      // float
    LIST_I64,      // int64_t
    LIST_U64,      // uint64_t
    LIST_F64,      // double
    LIST_PTR,      // void*
    LIST_STR,      // char*
    LIST_STRUCT,   // struct
} list_type_t;

typedef void (*list_deep_copy)( void** dest, const void* restrict src );    // if not specified, memcpy will be performed for LIST_STRUCT, shallow copy for all others
typedef void (*list_desctructor)( void** ptr );                             // ptr will be the address of that element, so void** for LIST_PTR

typedef void* (*list_malloc)( size_t size );                                        // malloc for custom allocator
typedef void* (*list_realloc)( void* old_ptr, size_t old_size, size_t new_size );   // realloc for custom allocator
typedef void  (*list_free)( void* ptr );                                            // free for custom alloc

typedef struct
{
    list_malloc    malloc;      // must be provided if a custom alloc is desired
    list_realloc   realloc;     // must be provided if a custom allocated is desired
    list_free      free;        // not necessary, because things like an arena alloc may not have a free function
} list_alloc_t;


typedef struct
{
    list_type_t        type;    // type of the elements
    size_t              size;   // size of a single element
    list_deep_copy     copy;    // copy constructor for custom element type
    list_desctructor   free;    // desctructor for custom element type
    list_alloc_t       alloc;   // cumstom alloc set for list
} list_args_t;



// functions
void*   list_new( list_args_t args );
void    list_delete( void* list );
size_t  list_len( void* list );                                                 // get the current size of the list. 
void*   list_resize( void* list, size_t size );                                 // Return new address if the array. You need to make sure data is valid if `list_desctructor` is set. 
void*   list_grow( void* list, size_t size );                                   // Return new address of the array. Do nothing if size is not bigger than current size. 
void*   list_shrink( void* list, size_t size );                                 // Return new address of the array. Do nothing if size is not smaller than current size. 
void*   list_append( void* list, /* T val */... );                              // with LIST_STRUCT, pass in the address of the struct. Return new address of the array. 
void*   list_insert( void* list, size_t index, size_t size, /* T* arr */... );  // Return new address of the array. 
void*   list_pop( void* list, size_t index );                                   // Return new address of the array. 





// list_new( list_type_t type, size_t size, list_deep_copy copy, list_desctructor free, list_alloc_t alloc );
#define list_new_args( ... )               list_new( (list_args_t) { __VA_ARGS__ } );            



#endif  // __LIST_H__