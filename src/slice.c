#include "slice.h"

typedef struct
{
    slice_type_t    type;
    size_t          size;
    size_t          capacity;
    size_t          per_size;
    slice_copy      copy;
    slice_free      free;
    char            data[];
} slice_t;


static inline bool slice_fit_cap( slice_t** src, size_t size )
{
    size_t cap;
    size_t capacity;
    if ( (*src)->capacity == 0 )
    {
        (*src)->capacity = 16;
        capacity = 0;
    }
    else
    {
    capacity = (*src)->capacity;
    }
    if ( size < 16 ) cap = 16;
    else cap = size;
    if ( cap >= (*src)->capacity )
    {
        while ( (*src)->capacity < cap )
        {
            if ( (*src)->capacity <= 1024 )
            {
                (*src)->capacity *= 2;
            }
            else
            {
                (*src)->capacity += 512;
            }
        }
        if ( (*src)->capacity == capacity ) return true;
        (*src) = realloc( (*src), sizeof ( slice_t ) + (*src)->per_size * ( (*src)->capacity ) );
        if ( (*src) == NULL ) return false;        
    }
    else
    {
        while ( cap > 1024 )
        {
            if ( cap + 512 < (*src)->capacity )
            {
                (*src)->capacity -= 512;
            }
            else break;
        }
        while ( cap <= 1024 )
        {
            if ( cap * 2 <= (*src)->capacity )
            {
                (*src)->capacity /= 2;
            }
            else break;
        }
        if ( (*src)->capacity == capacity ) return true;
        *src = realloc( *src, sizeof ( slice_t ) + (*src)->per_size * ( (*src)->capacity ) );
        if ( (*src) == NULL ) return false;
    }
    return true;
}


static inline slice_t* slice_meta( void* src )
{
    if ( src == NULL ) return ( fprintf( stderr, "[ERRO]: NULL pointer cannot be dereferenced.\n" ), exit(1), NULL );
    return (slice_t*) ( (char*) src - offsetof( slice_t, data ) );
}


void* slice_new( slice_args_t args )
{
    size_t size;
    switch ( args.type )
    {
        case SLICE_CHAR:    size = sizeof ( char );                         break;
        case SLICE_WCHAR:   size = sizeof ( wchar_t );                      break;
        case SLICE_I32:     size = sizeof ( int32_t );                      break;
        case SLICE_U32:     size = sizeof ( uint32_t );                     break;
        case SLICE_F32:     size = sizeof ( float );                        break;
        case SLICE_I64:     size = sizeof ( int64_t );                      break;
        case SLICE_U64:     size = sizeof ( uint64_t );                     break;
        case SLICE_F64:     size = sizeof ( double );                       break;
        case SLICE_PTR:     size = sizeof ( void* );                        break;
        case SLICE_STR:     size = sizeof ( char* );                        break;
        case SLICE_GENERIC: size = args.size;                               break;
        default:            fprintf( stderr, "[ERRO]: illegal type.\n" );   exit(1);
    }
    slice_t* slice = malloc( sizeof (slice_t) );
    *slice = (slice_t)
    {
        .type       = args.type,
        .size       = 0,
        .capacity   = 0,
        .per_size   = size,
        .copy       = args.copy,
        .free       = args.free,
    };
    if ( slice_fit_cap( &slice, 0 ) )
    {
        return (void*) slice->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void slice_delete( void* slice )
{
    slice_t* src = slice_meta( slice );
    if ( src->free != NULL )
    {
        for ( size_t i = 0; i < src->size; i++ )
        {
            src->free( (void**) ( src->data + src->per_size * i ) );
        }
    }
    free( src );
}


size_t slice_size( void* slice )
{
    slice_t* src = slice_meta( slice );
    return src->size;
}


void* slice_resize( void* slice, size_t size )
{
    return slice_grow( slice_shrink( slice, size ), size );
}


void* slice_grow( void* slice, size_t size )
{
    slice_t* src = slice_meta( slice );
    if ( size <= src->size )
    {
        return (void*) src->data;
    }
    if ( slice_fit_cap( &src, size ) )
    {
        src->size = size;
        return (void*) src->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* slice_shrink( void* slice, size_t size )
{
    slice_t* src = slice_meta( slice );
    if ( size >= src->size )
    {
        return (void*) src->data;
    }
    if ( src->free != NULL )
    {
        for ( size_t i = size; i < src->size; i++ )
        {
            src->free( (void**) ( src->data + src->per_size * i ) );
        }
    }
    if ( slice_fit_cap( &src, size ) )
    {
        src->size = size;
        return (void*) src->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


void* slice_append( void* slice, ... )
{
    va_list ap;
    va_start( ap, slice );
    slice_t* src = slice_meta( slice );
    if ( slice_fit_cap( &src, src->size + 1 ) )
    {
        if ( src->copy != NULL )
        {
            void* data = va_arg( ap, void* );
            src->copy( (void**) (src->data + src->per_size * src->size ), data );
        }
        else
        {
            switch ( src->type )
            {        
                case SLICE_CHAR:    ( (char*)       src->data )[ src->size ] = va_arg( ap, int );       break;
                case SLICE_WCHAR:   ( (wchar_t*)    src->data )[ src->size ] = va_arg( ap, wchar_t );   break;
                case SLICE_I32:     ( (int32_t*)    src->data )[ src->size ] = va_arg( ap, int32_t );   break;
                case SLICE_U32:     ( (uint32_t*)   src->data )[ src->size ] = va_arg( ap, uint32_t );  break;
                case SLICE_F32:     ( (float*)      src->data )[ src->size ] = va_arg( ap, double );    break;
                case SLICE_I64:     ( (int64_t*)    src->data )[ src->size ] = va_arg( ap, int64_t );   break;
                case SLICE_U64:     ( (uint64_t*)   src->data )[ src->size ] = va_arg( ap, uint64_t );  break;
                case SLICE_F64:     ( (double*)     src->data )[ src->size ] = va_arg( ap, double );    break;
                case SLICE_PTR:     ( (void**)      src->data )[ src->size ] = va_arg( ap, void* );     break;
                case SLICE_STR:     ( (char**)      src->data )[ src->size ] = va_arg( ap, void* );     break;
                case SLICE_GENERIC:
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


void* slice_insert( void* slice, size_t index, size_t size, ... )
{
    va_list ap;
    va_start( ap, size );
    slice_t* src = slice_meta( slice );
    if ( index > src->size )
    {
        return ( fprintf( stderr, "[ERRO]: insert index out of bounds.\n" ), src );
    }
    if ( slice_fit_cap( &src, src->size + size ) )
    {
        memmove( src->data + src->per_size * ( index + size ), src->data + src->per_size * index, src->per_size * ( src->size - index ) );
        void* data = va_arg( ap, void* );
        if ( src->copy != NULL )
        {
            for ( size_t i = 0; i < size; i++ )
            {
                src->copy( (void**) (src->data + src->per_size * ( index + i ) ), (char*) data + src->per_size * i );
            }
        }
        else
        {
            switch ( src->type )
            {        
                case SLICE_CHAR:    
                case SLICE_WCHAR:   
                case SLICE_I32:     
                case SLICE_U32:     
                case SLICE_F32:     
                case SLICE_I64:     
                case SLICE_U64:     
                case SLICE_F64:     
                case SLICE_PTR:
                case SLICE_STR:
                case SLICE_GENERIC:     
                    memmove( src->data + src->per_size * index, data, src->per_size * size );
                    break;
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


void* slice_pop( void* slice, size_t index )
{
    slice_t* src = slice_meta( slice );
    if ( index >= src->size )
    {
        return ( fprintf( stderr, "[ERRO]: pop index out of bounds.\n" ), slice );
    }
    if ( src->free != NULL )
    {
        src->free( (void**) src->data + src->per_size * index );
    }
    memmove( src->data + src->per_size * index, src->data + src->per_size * ( index + 1 ), src->per_size * ( src->size - index - 1 ) );
    if ( slice_fit_cap( &src, --src->size ) )
    {
        return (void*) src->data;
    }
    return ( fprintf( stderr, "[ERRO]: out of memory.\n" ), exit(1), NULL );
}


