#include "list.h"

#define DEFAULT_MINIMUM     64
#define DEFAULT_THRESHOLD   1024
#define DEFAULT_STEP_SIZE   512

typedef struct
{
    list_type_t         type;
    size_t              size;
    size_t              capacity;
    size_t              per_size;
    list_attr_t         attr;
    list_alloc_t        alloc;
    char                data[];
} list_t;


static inline void* default_realloc( void* restrict ptr, size_t old_size, size_t new_size )
{
    (void) old_size;
    return realloc( ptr, new_size );
}


static inline bool list_fit_cap( list_t** restrict src, size_t size )
{
    size_t cap;
    size_t capacity;
    if ( (*src)->capacity == 0 )
    {
        (*src)->capacity = DEFAULT_MINIMUM;
        capacity = 0;
    }
    else
    {
        capacity = (*src)->capacity;
    }
    size_t old_size = sizeof (list_t) + (*src)->per_size * capacity;
    if ( size < DEFAULT_MINIMUM ) cap = DEFAULT_MINIMUM;
    else cap = size;
    if ( cap >= (*src)->capacity )
    {
        while ( (*src)->capacity < cap )
        {
            if ( (*src)->capacity <= DEFAULT_THRESHOLD )
            {
                (*src)->capacity *= 2;
            }
            else
            {
                (*src)->capacity += DEFAULT_STEP_SIZE;
            }
        }
    }
    else
    {
        while ( cap > DEFAULT_THRESHOLD )
        {
            if ( cap + DEFAULT_STEP_SIZE < (*src)->capacity )
            {
                (*src)->capacity -= DEFAULT_STEP_SIZE;
            }
            else break;
        }
        while ( cap <= DEFAULT_THRESHOLD )
        {
            if ( cap * 2 <= (*src)->capacity )
            {
                (*src)->capacity /= 2;
            }
            else break;
        }
    }
    if ( (*src)->capacity == capacity ) return true;
    size_t new_size = sizeof ( list_t ) + (*src)->per_size * ( (*src)->capacity );
    (*src) = (*src)->alloc.realloc( (*src), old_size, new_size );
    if ( (*src) == NULL ) return false; 
    return true;
}


static inline list_t* list_meta( void* restrict src )
{
    if ( src == NULL ) return ( fprintf( stderr, "[ERRO]: NULL pointer cannot be dereferenced.\n" ), exit(1), NULL );
    return (list_t*) ( (char*) src - offsetof( list_t, data ) );
}


void* list_create( list_args_t args )
{
    size_t size;
    switch ( args.type )
    {
        case LIST_CHAR:    size = sizeof ( char );                         break;
        case LIST_WCHAR:   size = sizeof ( wchar_t );                      break;
        case LIST_I32:     size = sizeof ( int32_t );                      break;
        case LIST_U32:     size = sizeof ( uint32_t );                     break;
        case LIST_F32:     size = sizeof ( float );                        break;
        case LIST_I64:     size = sizeof ( int64_t );                      break;
        case LIST_U64:     size = sizeof ( uint64_t );                     break;
        case LIST_F64:     size = sizeof ( double );                       break;
        case LIST_PTR:     size = sizeof ( void* );                        break;
        case LIST_STR:     size = sizeof ( char* );                        break;
        case LIST_STRUCT:  size = args.size;                               break;
        default:            fprintf( stderr, "[ERRO]: illegal type.\n" );   exit(1);
    }
    list_t* list;
    if ( args.alloc.malloc != NULL && args.alloc.realloc != NULL )
    {
        list = args.alloc.malloc( sizeof (list_t) );
        if ( list == NULL ) return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
        *list = (list_t)
        {
            .type       = args.type,
            .size       = 0,
            .capacity   = 0,
            .per_size   = size,
            .attr       = args.attr,
            .alloc      = args.alloc,
        };
    }
    else
    {
        list = malloc( sizeof (list_t) );
        if ( list == NULL ) return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
        *list = (list_t)
        {
            .type       = args.type,
            .size       = 0,
            .capacity   = 0,
            .per_size   = size,
            .attr       = args.attr,
            .alloc      = 
            {
                .malloc     = malloc,
                .realloc    = default_realloc,
                .free       = free,
            },
        };
    }
    if ( list_fit_cap( &list, 0 ) )
    {
        return (void*) list->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void list_destroy( void* restrict list )
{
    list_t* src = list_meta( list );
    if ( src->attr.free != NULL )
    {
        for ( size_t i = 0; i < src->size; i++ )
        {
            src->attr.free( (void*) ( src->data + src->per_size * i ) );
        }
    }
    else if ( src->type == LIST_STR )
    {
        for ( size_t i = 0; i < src->size; i++ )
        {
            free( ( (char**) src->data )[i] );
        }
    }
    if ( src->alloc.free != NULL )
    {
        src->alloc.free( src );
    }
}


size_t list_len( void* restrict list )
{
    list_t* src = list_meta( list );
    return src->size;
}


void* list_resize( void* restrict list, size_t size )
{
    return list_grow( list_shrink( list, size ), size );
}


void* list_grow( void* restrict list, size_t size )
{
    list_t* src = list_meta( list );
    if ( size <= src->size )
    {
        return (void*) src->data;
    }
    if ( list_fit_cap( &src, size ) )
    {
        src->size = size;
        return (void*) src->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* list_shrink( void* restrict list, size_t size )
{
    list_t* src = list_meta( list );
    if ( size >= src->size )
    {
        return (void*) src->data;
    }
    if ( src->attr.free != NULL )
    {
        for ( size_t i = size; i < src->size; i++ )
        {
            src->attr.free( (void*) ( src->data + src->per_size * i ) );
        }
    }
    else if ( src->type == LIST_STR )
    {
        for ( size_t i = size; i < src->size; i++ )
        {
            free( ( (char**) src->data )[i] );
        }
    }
    if ( list_fit_cap( &src, size ) )
    {
        src->size = size;
        return (void*) src->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* list_append( void* restrict list, ... )
{
    va_list ap;
    va_start( ap, list );
    list_t* src = list_meta( list );
    if ( list_fit_cap( &src, src->size + 1 ) )
    {
        if ( src->attr.copy != NULL )
        {
            void* data = va_arg( ap, void* );
            src->attr.copy( ( src->data + src->per_size * src->size ), data );
        }
        else
        {
            switch ( src->type )
            {        
                case LIST_CHAR:    ( (char*)       src->data )[ src->size ] = va_arg( ap, int );       break;
                case LIST_WCHAR:   ( (wchar_t*)    src->data )[ src->size ] = va_arg( ap, int );       break;
                case LIST_I32:     ( (int32_t*)    src->data )[ src->size ] = va_arg( ap, int32_t );   break;
                case LIST_U32:     ( (uint32_t*)   src->data )[ src->size ] = va_arg( ap, uint32_t );  break;
                case LIST_F32:     ( (float*)      src->data )[ src->size ] = va_arg( ap, double );    break;
                case LIST_I64:     ( (int64_t*)    src->data )[ src->size ] = va_arg( ap, int64_t );   break;
                case LIST_U64:     ( (uint64_t*)   src->data )[ src->size ] = va_arg( ap, uint64_t );  break;
                case LIST_F64:     ( (double*)     src->data )[ src->size ] = va_arg( ap, double );    break;
                case LIST_PTR:     ( (void**)      src->data )[ src->size ] = va_arg( ap, void* );     break;
                case LIST_STR:
                {
                    char* str = va_arg( ap, char* );
                    ( (char**) src->data )[ src->size ] = strdup(str);
                    break;
                }
                case LIST_STRUCT:
                {
                    void* data = va_arg( ap, void* );
                    memcpy( src->data + src->per_size * src->size, data, src->per_size );
                    break;
                }
                default:
                {
                    return ( fprintf( stderr, "[ERRO]: illegal type.\n" ), exit(1), NULL );
                }
            }
        }
        src->size++;
        va_end( ap );
        return (void*) src->data;
    }
    va_end( ap );
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* list_insert( void* restrict list, size_t index, ... )
{
    va_list ap;
    va_start( ap, index );
    list_t* src = list_meta( list );
    if ( index > src->size )
    {
        return ( fprintf( stderr, "[ERRO]: insert index out of bounds.\n" ), src );
    }
    if ( list_fit_cap( &src, src->size + 1 ) )
    {
        memmove( src->data + src->per_size * ( index + 1 ), src->data + src->per_size * index, src->per_size * ( src->size - index ) );
        void* data = va_arg( ap, void* );
        if ( src->attr.copy != NULL )
        {
            src->attr.copy( ( src->data + src->per_size * index ), data );
        }
        else
        {
            switch ( src->type )
            {        
                case LIST_CHAR:    
                case LIST_WCHAR:   
                case LIST_I32:     
                case LIST_U32:     
                case LIST_F32:     
                case LIST_I64:     
                case LIST_U64:     
                case LIST_F64:     
                case LIST_PTR:
                case LIST_STRUCT:
                {
                    memmove( src->data + src->per_size * index, data, src->per_size );
                    break;
                }
                case LIST_STR:
                {
                   char* str = va_arg( ap, char* );
                    ( (char**) src->data )[ index ] = strdup(str);
                    break;
                }
                default:
                {
                    return ( fprintf( stderr, "[ERRO]: illegal type.\n" ), exit(1), NULL );
                }
            }
        }
        src->size++;
        va_end( ap );
        return (void*) src->data;
    }
    va_end( ap );
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* list_inserts( void* restrict list, size_t index, size_t size, ... )
{
    va_list ap;
    va_start( ap, size );
    list_t* src = list_meta( list );
    if ( index > src->size )
    {
        return ( fprintf( stderr, "[ERRO]: insert index out of bounds.\n" ), src );
    }
    if ( list_fit_cap( &src, src->size + size ) )
    {
        memmove( src->data + src->per_size * ( index + size ), src->data + src->per_size * index, src->per_size * ( src->size - index ) );
        void* data = va_arg( ap, void* );
        if ( src->attr.copy != NULL )
        {
            for ( size_t i = 0; i < size; i++ )
            {
                src->attr.copy( ( src->data + src->per_size * ( index + i ) ), (char*) data + ( src->per_size * i ) );
            }
        }
        else
        {
            switch ( src->type )
            {        
                case LIST_CHAR:    
                case LIST_WCHAR:   
                case LIST_I32:     
                case LIST_U32:     
                case LIST_F32:     
                case LIST_I64:     
                case LIST_U64:     
                case LIST_F64:     
                case LIST_PTR:
                case LIST_STRUCT:
                {
                    memmove( src->data + src->per_size * index, data, src->per_size * size );
                    break;
                }     
                case LIST_STR:
                {
                    for ( size_t i = 0; i < size; i++ )
                    {
                        ( (char**) src->data )[ index + i ] = strdup( ( (char**) data )[i] );
                    }
                    break;
                }
                default:
                {
                    return ( fprintf( stderr, "[ERRO]: illegal type.\n" ), exit(1), NULL );
                }
            }
        }
        src->size += size;
        va_end( ap );
        return (void*) src->data;
    }
    va_end( ap );
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* list_pop( void* restrict list, size_t index )
{
    list_t* src = list_meta( list );
    if ( index >= src->size )
    {
        return ( fprintf( stderr, "[ERRO]: pop index out of bounds.\n" ), list );
    }
    if ( src->attr.free != NULL )
    {
        src->attr.free( (void*) ( src->data + ( src->per_size * index ) ) );
    }
    else if ( src->type == LIST_STR )
    {
        free( ( (char**) src->data )[index] );
    }
    memmove( src->data + src->per_size * index, src->data + src->per_size * ( index + 1 ), src->per_size * ( src->size - index - 1 ) );
    if ( list_fit_cap( &src, --src->size ) )
    {
        return (void*) src->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* list_pops( void* restrict list, size_t index, size_t size )
{
    list_t* src = list_meta( list );
    if ( index + size >= src->size )
    {
        return ( fprintf( stderr, "[ERRO]: pop index out of bounds.\n" ), list );
    }
    if ( src->attr.free != NULL )
    {
        for ( size_t i = 0; i < size; i++ )
        {
            src->attr.free( (void*) ( src->data + ( src->per_size * ( index + i ) ) ) );
        }
    }
    else if ( src->type == LIST_STR )
    {
        for ( size_t i = 0; i < size; i++ )
        {
            free( ( (char**) src->data )[i] );
        }
    }
    memmove( src->data + src->per_size * index, src->data + src->per_size * ( index + size ), src->per_size * ( src->size - index - size ) );
    if ( list_fit_cap( &src, src->size -= size ) )
    {
        return (void*) src->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}
