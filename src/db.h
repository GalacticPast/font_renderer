#pragma once

// asci art font : rubi_font https://patorjk.com/software/taag

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <process.h>
#else
#include <unistd.h>
#endif

#if defined(__win32_) || defined(__WIN32) || defined(_WIN32)

#define DB_PLATFORM_WINDOWS
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif __linux__

#define DB_PLATFORM_LINUX
#include <fcntl.h>
#include <sanitizer/asan_interface.h>
#include <sys/mman.h>
#elif __APPLE__

#define DB_PLATFORM_MACOS
#include <sanitizer/asan_interface.h>
#include <sys/mman.h>

#endif

/*
   ▗▖  ▗▖▗▖  ▗▖    ▗▄▄▄▖▗▖  ▗▖▗▄▄▖ ▗▄▄▄▖ ▗▄▄▖
   ▐▛▚▞▜▌ ▝▚▞▘       █   ▝▚▞▘ ▐▌ ▐▌▐▌   ▐▌
   ▐▌  ▐▌  ▐▌        █    ▐▌  ▐▛▀▘ ▐▛▀▀▘ ▝▀▚▖
   ▐▌  ▐▌  ▐▌        █    ▐▌  ▐▌   ▐▙▄▄▖▗▄▄▞▘
   */
// types typedef
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef s8  b8;
typedef s32 b32;

typedef size_t s_size;

/*
   ▗▖ ▗▖ ▗▄▄▖▗▄▄▄▖▗▄▄▄▖▗▖ ▗▖▗▖       ▗▖  ▗▖ ▗▄▖  ▗▄▄▖▗▄▄▖  ▗▄▖  ▗▄▄▖
   ▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▌ ▐▌▐▌       ▐▛▚▞▜▌▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌
   ▐▌ ▐▌ ▝▀▚▖▐▛▀▀▘▐▛▀▀▘▐▌ ▐▌▐▌       ▐▌  ▐▌▐▛▀▜▌▐▌   ▐▛▀▚▖▐▌ ▐▌ ▝▀▚▖
   ▝▚▄▞▘▗▄▄▞▘▐▙▄▄▖▐▌   ▝▚▄▞▘▐▙▄▄▖    ▐▌  ▐▌▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▝▚▄▞▘▗▄▄▞▘
*/
#define DEBUG_BREAK __builtin_trap()

#define ASSERT(expr)                                                                                                   \
    {                                                                                                                  \
        do                                                                                                             \
        {                                                                                                              \
            if (!(expr))                                                                                               \
            {                                                                                                          \
                printf("ASSERTion failure: %s:%d on %s\n", __FILE__, __LINE__, #expr);                                 \
                DEBUG_BREAK;                                                                                           \
            }                                                                                                          \
        } while (0);                                                                                                   \
    }

#define ASSERT_WITH_MSG(expr, msg)                                                                                     \
    {                                                                                                                  \
        do                                                                                                             \
        {                                                                                                              \
            if (!(expr))                                                                                               \
            {                                                                                                          \
                printf("%s\n.", msg);                                                                                  \
                printf("ASSERTion failure: %str:%d on %s\n", __FILE__, __LINE__, #expr);                               \
                DEBUG_BREAK;                                                                                           \
            }                                                                                                          \
        } while (0);                                                                                                   \
    }

#define false 0
#define true 1

#define s32_min -2147483648
#define s32_max 2147483647

// hmmmm we need a floating point max too ?
#define db_max(n, m) (s64) n >= (s64)m ? (s64)n : (s64)m
#define db_min(n, m) (s64) n <= (s64)m ? (s64)n : (s64)m

#define KB(n) ((s32)n * 1024)
#define MB(n) ((s32)n * 1024 * 1024)
#define GB(n) ((s32)n * 1024 * 1024 * 1024)

#define pi32 3.1415926535897f
#define machine_epsilon64 4.94065645841247e-324

#define max_u64 0xffffffffffffffffull
#define max_u32 0xffffffff
#define max_u16 0xffff
#define max_u8 0xff

#define max_s64 ((s64)0x7fffffffffffffffll)
#define max_s32 ((s32)0x7fffffff)
#define max_s16 ((s16)0x7fff)
#define max_s8 ((s8)0x7f)

#define min_s64 ((s64)0x8000000000000000ll)
#define min_s32 ((s32)0x80000000)
#define min_s16 ((s16)0x8000)
#define min_s8 ((s8)0x80)

#define db_clamp_integer(min, val_to_clamp, max) val_to_clamp >= min ? (val_to_clamp >= max ? max : val_to_clamp) : min

#define defer_loop(a, b) for (b8 i = 0, res = a; i != 1 && res; i++, b)

#define bitmask1 0x00000001
#define bitmask2 0x00000003
#define bitmask3 0x00000007
#define bitmask4 0x0000000f
#define bitmask5 0x0000001f
#define bitmask6 0x0000003f
#define bitmask7 0x0000007f
#define bitmask8 0x000000ff
#define bitmask9 0x000001ff
#define bitmask10 0x000003ff
#define bitmask11 0x000007ff
#define bitmask12 0x00000fff
#define bitmask13 0x00001fff
#define bitmask14 0x00003fff
#define bitmask15 0x00007fff
#define bitmask16 0x0000ffff
#define bitmask17 0x0001ffff
#define bitmask18 0x0003ffff
#define bitmask19 0x0007ffff
#define bitmask20 0x000fffff
#define bitmask21 0x001fffff
#define bitmask22 0x003fffff
#define bitmask23 0x007fffff
#define bitmask24 0x00ffffff
#define bitmask25 0x01ffffff
#define bitmask26 0x03ffffff
#define bitmask27 0x07ffffff
#define bitmask28 0x0fffffff
#define bitmask29 0x1fffffff
#define bitmask30 0x3fffffff
#define bitmask31 0x7fffffff
#define bitmask32 0xffffffff

#define bitmask33 0x00000001ffffffffull
#define bitmask34 0x00000003ffffffffull
#define bitmask35 0x00000007ffffffffull
#define bitmask36 0x0000000fffffffffull
#define bitmask37 0x0000001fffffffffull
#define bitmask38 0x0000003fffffffffull
#define bitmask39 0x0000007fffffffffull
#define bitmask40 0x000000ffffffffffull
#define bitmask41 0x000001ffffffffffull
#define bitmask42 0x000003ffffffffffull
#define bitmask43 0x000007ffffffffffull
#define bitmask44 0x00000fffffffffffull
#define bitmask45 0x00001fffffffffffull
#define bitmask46 0x00003fffffffffffull
#define bitmask47 0x00007fffffffffffull
#define bitmask48 0x0000ffffffffffffull
#define bitmask49 0x0001ffffffffffffull
#define bitmask50 0x0003ffffffffffffull
#define bitmask51 0x0007ffffffffffffull
#define bitmask52 0x000fffffffffffffull
#define bitmask53 0x001fffffffffffffull
#define bitmask54 0x003fffffffffffffull
#define bitmask55 0x007fffffffffffffull
#define bitmask56 0x00ffffffffffffffull
#define bitmask57 0x01ffffffffffffffull
#define bitmask58 0x03ffffffffffffffull
#define bitmask59 0x07ffffffffffffffull
#define bitmask60 0x0fffffffffffffffull
#define bitmask61 0x1fffffffffffffffull
#define bitmask62 0x3fffffffffffffffull
#define bitmask63 0x7fffffffffffffffull
#define bitmask64 0xffffffffffffffffull

#define bit0 0
#define bit1 (1u << 0)
#define bit2 (1u << 1)
#define bit3 (1u << 2)
#define bit4 (1u << 3)
#define bit5 (1u << 4)
#define bit6 (1u << 5)
#define bit7 (1u << 6)
#define bit8 (1u << 7)
#define bit9 (1u << 8)
#define bit10 (1u << 9)
#define bit11 (1u << 10)
#define bit12 (1u << 11)
#define bit13 (1u << 12)
#define bit14 (1u << 13)
#define bit15 (1u << 14)
#define bit16 (1u << 15)
#define bit17 (1u << 16)
#define bit18 (1u << 17)
#define bit19 (1u << 18)
#define bit20 (1u << 19)
#define bit21 (1u << 20)
#define bit22 (1u << 21)
#define bit23 (1u << 22)
#define bit24 (1u << 23)
#define bit25 (1u << 24)
#define bit26 (1u << 25)
#define bit27 (1u << 26)
#define bit28 (1u << 27)
#define bit29 (1u << 28)
#define bit30 (1u << 29)
#define bit31 (1u << 30)
#define bit32 (1u << 31)

#define bit33 (1ull << 32)
#define bit34 (1ull << 33)
#define bit35 (1ull << 34)
#define bit36 (1ull << 35)
#define bit37 (1ull << 36)
#define bit38 (1ull << 37)
#define bit39 (1ull << 38)
#define bit40 (1ull << 39)
#define bit41 (1ull << 40)
#define bit42 (1ull << 41)
#define bit43 (1ull << 42)
#define bit44 (1ull << 43)
#define bit45 (1ull << 44)
#define bit46 (1ull << 45)
#define bit47 (1ull << 46)
#define bit48 (1ull << 47)
#define bit49 (1ull << 48)
#define bit50 (1ull << 49)
#define bit51 (1ull << 50)
#define bit52 (1ull << 51)
#define bit53 (1ull << 52)
#define bit54 (1ull << 53)
#define bit55 (1ull << 54)
#define bit56 (1ull << 55)
#define bit57 (1ull << 56)
#define bit58 (1ull << 57)
#define bit59 (1ull << 58)
#define bit60 (1ull << 59)
#define bit61 (1ull << 60)
#define bit62 (1ull << 61)
#define bit63 (1ull << 62)
#define bit64 (1ull << 63)

// thanks google https://github.com/google/sanitizers/wiki/addresssanitizermanualpoisoning
// user code should use macros instead of functions.
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#define asan_poison_memory_region(addr, size) __asan_poison_memory_region((addr), (size))
#define asan_unpoison_memory_region(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
#define asan_poison_memory_region(addr, size) ((void)(addr), (void)(size))
#define asan_unpoison_memory_region(addr, size) ((void)(addr), (void)(size))
#endif

typedef enum db_return_code
{
    DB_ERROR   = 0,
    DB_SUCCESS = 1,
} db_return_code;

/*
▗▖  ▗▖▗▄▄▄▖▗▖  ▗▖ ▗▄▖ ▗▄▄▖▗▖  ▗▖
▐▛▚▞▜▌▐▌   ▐▛▚▞▜▌▐▌ ▐▌▐▌ ▐▌▝▚▞▘
▐▌  ▐▌▐▛▀▀▘▐▌  ▐▌▐▌ ▐▌▐▛▀▚▖ ▐▌
▐▌  ▐▌▐▙▄▄▖▐▌  ▐▌▝▚▄▞▘▐▌ ▐▌ ▐▌
*/

void          *__db_reserve_virtual_memory(size_t reserve_memory_size);
db_return_code __db_commit_virtual_memory(void *memory, s32 page_offset, s32 num_pages);
db_return_code __db_decommit_virtual_memory(void *memory, size_t size);
db_return_code __db_release_virtual_memory(void *memory, size_t size);

#if defined(DB_PLATFORM_LINUX) || defined(DB_PLATFORM_WINDOWS)
#define DB_PAGE_SIZE KB(4)
#elif defined(DB_PLATFORM_MACOS) // if the mac is an apple silicon which has an default page size of 16kb
// kind of a hack, what if the macos is apple intel ?
#define DB_PAGE_SIZE KB(16)
#endif
/*
 ▗▄▖ ▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖ ▗▄▖  ▗▄▄▖
▐▌ ▐▌▐▌ ▐▌▐▌   ▐▛▚▖▐▌▐▌ ▐▌▐▌
▐▛▀▜▌▐▛▀▚▖▐▛▀▀▘▐▌ ▝▜▌▐▛▀▜▌ ▝▀▚▖
▐▌ ▐▌▐▌ ▐▌▐▙▄▄▖▐▌  ▐▌▐▌ ▐▌▗▄▄▞▘
*/
typedef enum
{
    TYPE_ARENA_LINEAR  = 0,
    TYPE_ARENA_CHUNKED = 1, // free list arena
} db_arena_type;

typedef struct db_arena_params
{
    db_arena_type type;
    u32           chunk_size;
} db_arena_params;

typedef struct db_arena_chunk_header
{
    struct db_arena_chunk_header *next_chunk;
} db_arena_chunk_header;

typedef struct db_arena
{
    db_arena_type type;
    // how many pages has the arena commited till now.
    // we need this because if we need the arena to grow it will use the page offset to find out how many pages to
    // commit.
    s64    curr_page_offset;
    size_t allocated_till_now;
    size_t total_size;
    //  @ = memory in use/ already allocated
    //  @@@@@@@@@@@@--------------------
    //              ^-- starting ptr for the next allocation
    uintptr_t curr_mem_pos;
    // original or the first ptr, this is also the starting page'str memory ptr. we will need to pass this ptr if we
    // want to unmap the memory.
    void *memory; // this will also be the first chunk in the list

    // if its type arena chunked
    size_t                 allocated_chunk_count;
    u64                    chunk_size;
    db_arena_chunk_header *free_list_head;
} db_arena;

#define DB_ARENA_CHUNK_HEADER(ptr) (db_arena_chunk_header *)((uintptr_t)ptr - sizeof(db_arena_chunk_header))

#define DB_ARENA_DEFAULT_RESERVED_MEMORY MB(64)
#define DB_ARENA_DEFAULT_COMMITED_MEMORY DB_PAGE_SIZE
#define db_arena_init(...)                                                                                             \
    db_arena_init_with_size(&(db_arena_params){__VA_ARGS__}, (size_t)DB_ARENA_DEFAULT_COMMITED_MEMORY)
db_arena       db_arena_init_with_size(db_arena_params *params, size_t memory_size);
void          *db_arena_alloc(db_arena *arena, size_t size);
db_return_code db_arena_reset(db_arena *arena);
db_return_code db_arena_free(db_arena *arena);
db_return_code db_arena_free_node(db_arena *arena, void *ptr);
/*

▗▄▄▄▗▖  ▗▖▗▖  ▗▖ ▗▄▖ ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖     ▗▄▖ ▗▄▄▖ ▗▄▄▖  ▗▄▖▗▖  ▗▖▗▄▄▖
▐▌  █▝▚▞▘ ▐▛▚▖▐▌▐▌ ▐▌▐▛▚▞▜▌  █  ▐▌       ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▝▚▞▘▐▌
▐▌  █ ▐▌  ▐▌ ▝▜▌▐▛▀▜▌▐▌  ▐▌  █  ▐▌       ▐▛▀▜▌▐▛▀▚▖▐▛▀▚▖▐▛▀▜▌ ▐▌  ▝▀▚▖
▐▙▄▄▀ ▐▌  ▐▌  ▐▌▐▌ ▐▌▐▌  ▐▌▗▄█▄▖▝▚▄▄▖    ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌ ▐▌ ▗▄▄▞▘
dynamic arrays
*/
// inspo :
// @note: thanks ginger bill :) he is the inspiration for this whole single header file
//  https://github.com/gingerBill/gb/blob/master/gb.h
// https://iafisher.com/blog/2020/06/type-safe-generics-in-c

typedef struct db_array_skeleton
{
    s64       total_length;
    s64       length;
    s64       type_size;
    db_arena *arena;
    void     *data;
} db_array_skeleton;

#define DB_ARRAY_DEFAULT_RESIZE_FACTOR 2
#define DB_ARRAY_DEFAULT_ALLOCATION_BUCKETS 512

#define db_array_decl(name, Type)                                                                                      \
    typedef struct db_array_##name                                                                                     \
    {                                                                                                                  \
        s64       total_length;                                                                                        \
        s64       length;                                                                                              \
        s64       type_size;                                                                                           \
        db_arena *arena;                                                                                               \
        Type     *data;                                                                                                \
    } db_array_##name;                                                                                                 \
    static inline db_array_##name db_array_##name##_init(db_arena *db_arena_ptr)                                       \
    {                                                                                                                  \
        ASSERT_WITH_MSG(db_arena_ptr, "Passed Arena is Null.");                                                        \
        ASSERT_WITH_MSG(db_arena_ptr->type == TYPE_ARENA_LINEAR,                                                       \
                        "Arena has to be a linear, we dont support chunked arenas yet:)");                             \
        db_array_##name _array = {};                                                                                   \
        __db_array_init(db_arena_ptr, (db_array_skeleton *)&_array, sizeof(Type));                                     \
        return _array;                                                                                                 \
    }                                                                                                                  \
    static inline s64 db_array_##name##_length(db_array_##name *a)                                                     \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        return a->length;                                                                                              \
    }                                                                                                                  \
    static inline void db_array_##name##_set_length(db_array_##name *a, s64 n)                                         \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        a->length = n;                                                                                                 \
    }                                                                                                                  \
    static inline s64 db_array_##name##_capacity(db_array_##name *a)                                                   \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        return a->total_length / a->type_size;                                                                         \
    }                                                                                                                  \
    static inline void db_array##name##_free(db_array_##name *a)                                                       \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        __db_array_free((db_array_skeleton *)a);                                                                       \
    }                                                                                                                  \
    static inline Type db_array_##name##_get_index(db_array_##name *a, s64 index)                                      \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        ASSERT_WITH_MSG(index < a->length, "Index out of bounds.");                                                    \
        return a->data[index];                                                                                         \
    }                                                                                                                  \
    static inline Type *db_array_##name##_get_index_ptr(db_array_##name *a, s64 index)                                 \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        ASSERT_WITH_MSG(index < a->length, "Index out of bounds.");                                                    \
        return &a->data[index];                                                                                        \
    }                                                                                                                  \
    static inline void db_array_##name##_append(db_array_##name *a, Type element)                                      \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        if (a->length + 1 >= a->total_length)                                                                          \
        {                                                                                                              \
            __db_array_resize((db_array_skeleton *)a);                                                                 \
        }                                                                                                              \
        a->data[a->length++] = element;                                                                                \
    }                                                                                                                  \
    static inline void db_array_##name##__pop(db_array_##name *a)                                                      \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        ASSERT_WITH_MSG(a->length != 0, "array has no elements yet.");                                                 \
        a->length--;                                                                                                   \
    }                                                                                                                  \
    static inline void db_array_##name##_remove_range(db_array_##name *a, s64 start, s64 num_elems)                    \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        /*get the remaining length of the array after start + num_elems                                                \
         *  for example: start = 2, num_elems = 3, v = n0 n1 n2 n3 n4 n5 n6 ... n                                      \
         *     v = n0 n1 n2 n3 n4 n5 n6 ...    we want to remove elmems n2, n3, n4                                     \
         * to get the length of the n5+ array. we have to do v.count -   len(n0,n1,...n4)                              \
         * length of the whole array - the length from the starting of array up to the last element we want to remove  \
         * */                                                                                                          \
        ASSERT(start < a->length);                                                                                     \
        s64 rem_length = a->length - ((start) + (num_elems));                                                          \
        ASSERT(rem_length >= 0);                                                                                       \
        memmove(&a->data[(start)], &a->data[(start) + (num_elems)], rem_length * a->type_size);                        \
        a->length -= num_elems;                                                                                        \
        memset(&a->data[a->length], 0, num_elems * a->type_size);                                                      \
    }                                                                                                                  \
    static inline void db_array_##name##_insert(db_array_##name *a, s64 index, Type element)                           \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        if (a->length + 1 >= a->total_length)                                                                          \
        {                                                                                                              \
            __db_array_resize((db_array_skeleton *)&a);                                                                \
        }                                                                                                              \
        s64 move_length = a->length - index;                                                                           \
        memmove(&a->data[index + 1], &a->data[index], move_length * a->type_size);                                     \
        a->data[index] = element;                                                                                      \
        a->length++;                                                                                                   \
    }                                                                                                                  \
    static inline Type db_array_##name##_get_last(db_array_##name *a)                                                  \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        return a->data[a->length - 1];                                                                                 \
    }                                                                                                                  \
    static inline Type *db_array_##name##_get_last_ptr(db_array_##name *a)                                             \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        return &a->data[a->length - 1];                                                                                \
    }                                                                                                                  \
    static inline Type *db_array_##name##_find(db_array_##name *a, Type *elem,                                         \
                                               b8 (*db_array_##name##_cmp)(Type * a, Type * b))                        \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        s64 length = a->length;                                                                                        \
        for (s64 i = 0; i < length; i++)                                                                               \
        {                                                                                                              \
            if (db_array_##name##_cmp(elem, &a->data[i]))                                                              \
            {                                                                                                          \
                return &a->data[i];                                                                                    \
            }                                                                                                          \
        }                                                                                                              \
        return NULL;                                                                                                   \
    }                                                                                                                  \
    static inline db_array_##name db_array_##name##_duplicate(db_array_##name *a)                                      \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        db_array_##name _res   = db_array_##name##_init(a->arena);                                                     \
        s64             length = a->length;                                                                            \
        for (s64 i = 0; i < length; i++)                                                                               \
        {                                                                                                              \
            db_array_##name##_append(&_res, a->data[i]);                                                               \
        }                                                                                                              \
        return _res;                                                                                                   \
    }                                                                                                                  \
    static inline void db_array_##name##_clear(db_array_##name *a)                                                     \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        memset(a->data, 0, a->length * a->type_size);                                                                  \
        a->length = 0;                                                                                                 \
    }                                                                                                                  \
    static inline void db_array_##name##_copy(db_array_##name *arr_dest, db_array_##name *arr_src)                     \
    {                                                                                                                  \
        ASSERT(arr_dest);                                                                                              \
        ASSERT(arr_src);                                                                                               \
        db_array_##name##_clear(arr_dest);                                                                             \
        s64 length = db_array_##name##_length(arr_src);                                                                \
        for (s64 i = 0; i < length; i++)                                                                               \
        {                                                                                                              \
            db_array_##name##_append(arr_dest, arr_src->data[i]);                                                      \
        }                                                                                                              \
    }

// removes from the starting index + num_elems elements
#define db_array_for_each(array, i, iter) for (i = 0, iter = array.data[i]; i < array.length; i++, iter = array.data[i])
#define db_array_for_each_ptr(array, i, iter)                                                                          \
    for (i = 0, iter = &array.data[i]; i < array.length; i++, iter = &array.data[i])

db_return_code __db_array_init(db_arena *shared_arena, db_array_skeleton *array, size_t type_size);
void           __db_array_free(db_array_skeleton *array);
db_return_code __db_array_resize(db_array_skeleton *array);

/*
  ▗▄▄▖▗▄▄▄▖▗▄▖  ▗▄▄▖▗▖ ▗▖
 ▐▌     █ ▐▌ ▐▌▐▌   ▐▌▗▞▘
  ▝▀▚▖  █ ▐▛▀▜▌▐▌   ▐▛▚▖
 ▗▄▄▞▘  █ ▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌

// */
typedef struct db_stack_skeleton
{
    s64       total_length;
    s64       length;
    s64       type_size;
    db_arena *arena;
    void     *data;
} db_stack_skeleton;

#define db_stack_decl(name, Type)                                                                                      \
    typedef struct db_stack_##name                                                                                     \
    {                                                                                                                  \
        s64       total_length;                                                                                        \
        s64       length;                                                                                              \
        s64       type_size;                                                                                           \
        db_arena *arena;                                                                                               \
        Type     *data;                                                                                                \
    } db_stack_##name;                                                                                                 \
    static inline db_stack_##name db_stack_##name##_init(db_arena *db_arena_ptr)                                       \
    {                                                                                                                  \
        ASSERT_WITH_MSG(db_arena_ptr, "Passed Arena is Null.");                                                        \
        ASSERT_WITH_MSG(db_arena_ptr->type == TYPE_ARENA_LINEAR,                                                       \
                        "Arena has to be a linear, we dont support chunked arenas yet:)");                             \
        db_stack_##name _stack = {};                                                                                   \
        __db_stack_init(db_arena_ptr, (db_stack_skeleton *)&_stack, sizeof(Type));                                     \
        return _stack;                                                                                                 \
    }                                                                                                                  \
    static inline s64 db_stack_##name##_length(db_stack_##name *a)                                                     \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        return a->length;                                                                                              \
    }                                                                                                                  \
    static inline void db_stack_##name##_push(db_stack_##name *a, Type element)                                        \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        if (a->length + 1 >= a->total_length)                                                                          \
        {                                                                                                              \
            __db_stack_resize((db_stack_skeleton *)a);                                                                 \
        }                                                                                                              \
        a->data[a->length++] = element;                                                                                \
    }                                                                                                                  \
    static inline void db_stack_##name##_pop(db_stack_##name *a)                                                       \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        ASSERT_WITH_MSG(a->length != 0, "stack has no elements yet.");                                                 \
        a->length--;                                                                                                   \
    }                                                                                                                  \
    static inline Type db_stack_##name##_peek(db_stack_##name *a)                                                      \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        return a->data[a->length - 1];                                                                                 \
    }                                                                                                                  \
    static inline Type *db_stack_##name##_peek_ptr(db_stack_##name *a)                                                 \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        return &a->data[a->length - 1];                                                                                \
    }                                                                                                                  \
    static inline void db_stack##name##_free(db_stack_##name *a)                                                       \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        __db_stack_free((db_stack_skeleton *)a);                                                                       \
    }                                                                                                                  \
    static inline void db_stack_##name##_clear(db_stack_##name *a)                                                     \
    {                                                                                                                  \
        ASSERT(a);                                                                                                     \
        memset(a->data, 0, a->length * a->type_size);                                                                  \
        a->length = 0;                                                                                                 \
    }

db_return_code __db_stack_init(db_arena *shared_arena, db_stack_skeleton *array, size_t type_size);
db_return_code __db_stack_resize(db_stack_skeleton *stack);
void           __db_stack_free(db_stack_skeleton *stack);
// /*
//  ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▖  ▗▄▄▖
// ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌
// ▐▌   ▐▛▀▜▌▐▛▀▜▌▐▛▀▚▖ ▝▀▚▖
// ▝▚▄▄▖▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▗▄▄▞▘
//
// */
//
char db_char_to_lower(char c);
char db_char_to_upper(char c);
b8   db_char_is_space(char c);
b8   db_char_is_digit(char c);
b8   db_char_is_hex_digit(char c);
b8   db_char_is_alphabet(char c);
b8   db_char_is_alphanumeric(char c);
s32  db_digit_to_int(char c);
s32  db_hex_digit_to_int(char c);

void db_str_to_lower(char *str);
void db_str_to_upper(char *str);

char const *db_char_first_occurence(char const *str, char c);
char const *db_char_last_occurence(char const *str, char c);
//
// /*
//      ▗▄▄▖▗▄▄▄▖▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖ ▗▄▄▖
//     ▐▌     █  ▐▌ ▐▌  █  ▐▛▚▖▐▌▐▌   ▐▌
//      ▝▀▚▖  █  ▐▛▀▚▖  █  ▐▌ ▝▜▌▐▌▝▜▌ ▝▀▚▖
//     ▗▄▄▞▘  █  ▐▌ ▐▌▗▄█▄▖▐▌  ▐▌▝▚▄▞▘▗▄▄▞▘
//
// */
// // https://graphics.stanford.edu/~seander/bithacks.html#ZeroInWord
// // all the tricks that I use for the following funcs are defined in this above page
// // Still have to implement it
size_t db_strlen(char const *str);
// // size_t db_strnlen(char const *str, size_t max_len);
s32 db_strcmp(char const *s1, char const *s2);
// // s32    db_strncmp(char const *s1, char const *s2, size_t len);
// // char  *db_strcpy(char *dest, char const *source);
// // char  *db_strncpy(char *dest, char const *source, size_t len);
// // size_t db_strlcpy(char *dest, char const *source, size_t len);
// // char  *db_strrev(char *str); // NOTE(bill): ASCII only
// //
// // char const *db_strtok(char *output, char const *src, char const *delimit);
// //
// // b8 db_str_has_prefix(char const *str, char const *prefix);
// // b8 db_str_has_suffix(char const *str, char const *suffix);
// //
// //
// // void gb_str_concat(char *dest, size_t dest_len, char const *src_a, size_t src_a_len, char const *src_b,
// //                   size_t src_b_len);
// //
// // u64  db_str_to_u64(char const *str, char **end_ptr,
// //                   s32 base); // TODO: Support more than just decimal and hexadecimal
// // s64  db_str_to_i64(char const *str, char **end_ptr,
// //                   s32 base); // TODO: Support more than just decimal and hexadecimal
// // f32  db_str_to_f32(char const *str, char **end_ptr);
// // f64  db_str_to_f64(char const *str, char **end_ptr);
// // void db_i64_to_str(s64 value, char *string, s32 base);
// // void db_u64_to_str(u64 value, char *string, s32 base);
//

typedef struct db_string
{
    s64       capacity;
    s64       length;
    db_arena *arena;
    char     *data; // this is actually a linked list
} db_string;

// this is for the initial capacity for strings whose arenas are TYPE_ARENA_LINEAR
#define DB_STRING_LINEAR_SIZE 512

char *db_string_get_first_occurence(const char *str, const char *sub);
void  db_string_make_reserve(db_string str, s64 capacity);

db_string db_string_make(db_arena *arena, char const *a);
db_string db_string_duplicate(db_arena *arena, db_string const *const str);
// does not do anything except if its a pool allocator
void db_string_free(db_string *const str);
s64  db_string_length(db_string const *const str);
void db_string_clear(db_string *const str);
void db_string_append(db_string *const str, const char *other);
void db_string_append_char(db_string *const str, const char c);
b8   db_strings_are_equal(db_string const *lhs, db_string const *rhs);
//@todo:incomplete
// db_string db_string_trim(db_arena *scratch_arena, db_string *const str, char const *cut_set);
// db_string db_string_trim_space(db_arena *scratch_arena, db_string *const str); // Whitespace ` \t\r\n\v\f`
char *db_string_get_cstr(db_arena *arena, db_string const *const str);
void  db_string_set(db_string *const str, char const *cstr);

// well this looks like for utf8 strings. Well Let me figure that out later
// db_string db_string_append_rune(db_string str, Rune r);
// I have to have my own fmt parser so this is disabled for now
// db_string db_string_append_fmt(db_string str, char const *fmt, ...);
// hmmmmm I dont think we need this because our arena will automatically scale based on appending
// db_string db_string_make_length(db_string str, void const *a, s64 num_bytes);
// void       db_string_make_space_for(db_string str, s64 add_len);
// s64        db_string_allocation_size(db_string const str);
/*
// ▗▖ ▗▖ ▗▄▖  ▗▄▄▖▗▖ ▗▖▗▖  ▗▖ ▗▄▖ ▗▄▄▖
// ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌ ▐▌▐▛▚▞▜▌▐▌ ▐▌▐▌ ▐▌
// ▐▛▀▜▌▐▛▀▜▌ ▝▀▚▖▐▛▀▜▌▐▌  ▐▌▐▛▀▜▌▐▛▀▘
// ▐▌ ▐▌▐▌ ▐▌▗▄▄▞▘▐▌ ▐▌▐▌  ▐▌▐▌ ▐▌▐▌
// */
//
#define DB_HASH_SEED 0x31415926
#define db_hash_string(data) db_murmur64A_seed(data, strlen(data), DB_HASH_SEED)
u64 db_murmur64A_seed(void const *const key, u64 len, u64 seed);

// os stuff

typedef u32 db_file_mode;
typedef enum db_file_mode_flag
{
    db_file_mode_read   = bit0,
    db_file_mode_write  = bit1,
    db_file_mode_append = bit2,
    db_file_mode_rw     = bit3,
    db_file_mode_rb     = bit4,

    db_file_mode_modes = db_file_mode_read | db_file_mode_write | db_file_mode_append | db_file_mode_rw,
} db_file_mode_flag;

typedef enum db_file_error
{
    db_file_error_none,
    db_file_error_invalid,
    db_file_error_invalid_filename,
    db_file_error_exists,
    db_file_error_not_exists,
    db_file_error_permission,
    db_file_error_truncation_failure,
} db_file_error;

typedef union db_file_descriptor {
    void     *p;
    intptr_t  i;
    uintptr_t u;
} db_file_descriptor;

typedef u64 db_file_time;

typedef struct db_file
{
    db_file_descriptor fd;
    char const        *filename;
    db_file_time       last_write_time;
} db_file;

typedef struct db_file_contents
{
    db_arena *arena;
    void     *data;
    s_size    size;
} db_file_contents;

db_file_error db_file_exists(char const *filepath);
// @fix:
// well I have to change it later on
FILE *db_file_open(char const *filepath, db_file_mode mode);

db_file_contents db_file_read_contents(db_arena *a, db_file_mode mode, b32 zero_terminate, char const *filepath);
void             db_file_free_contents(db_file_contents *fc);

#ifndef DB_PATH_SEPARATOR
#if defined(DB_SYSTEM_WINDOWS)
#define DB_PATH_SEPARATOR '\\'
#else
#define DB_PATH_SEPARATOR '/'
#endif
#endif

/*
▗▄▄▄▖▗▖  ▗▖▗▄▄▖ ▗▖   ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▖▗▄▄▄▖▗▄▄▄▖ ▗▄▖ ▗▖  ▗▖
  █  ▐▛▚▞▜▌▐▌ ▐▌▐▌   ▐▌   ▐▛▚▞▜▌▐▌   ▐▛▚▖▐▌  █ ▐▌ ▐▌ █    █  ▐▌ ▐▌▐▛▚▖▐▌
  █  ▐▌  ▐▌▐▛▀▘ ▐▌   ▐▛▀▀▘▐▌  ▐▌▐▛▀▀▘▐▌ ▝▜▌  █ ▐▛▀▜▌ █    █  ▐▌ ▐▌▐▌ ▝▜▌
▗▄█▄▖▐▌  ▐▌▐▌   ▐▙▄▄▖▐▙▄▄▖▐▌  ▐▌▐▙▄▄▖▐▌  ▐▌  █ ▐▌ ▐▌ █  ▗▄█▄▖▝▚▄▞▘▐▌  ▐▌

*/

#ifdef DB_IMPLEMENTATION
#undef DB_IMPLEMENTATION
// verify later on though if i could have huge pages or not

void *__db_reserve_virtual_memory(size_t reserve_memory_size)
{
    void *ptr = NULL;
#if defined(DB_PLATFORM_LINUX) || defined(DB_PLATFORM_MACOS)
    // thanks @tsoding (mista zozin) for the mmap explanation https://youtu.be/sfyfubzu9ow
    ptr = mmap(NULL, reserve_memory_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    ASSERT(ptr != MAP_FAILED);

    return ptr;
#elif defined(DB_PLATFORM_WINDOWS)
    ptr = VirtualAlloc(0, reserve_memory_size, MEM_RESERVE, PAGE_READWRITE);
#endif
    ASSERT(ptr != NULL);
    return (void *)ptr;
}

db_return_code __db_commit_virtual_memory(void *memory, s32 page_offset, s32 num_pages)
{
    uintptr_t next_page_base_ptr = (uintptr_t)memory + (page_offset * DB_PAGE_SIZE);
    s64       new_allocated_size = num_pages * DB_PAGE_SIZE;

#if defined(DB_PLATFORM_LINUX) || defined(DB_PLATFORM_MACOS)
    s32 ret_code = mprotect((void *)next_page_base_ptr, new_allocated_size, PROT_READ | PROT_WRITE);
    if (ret_code == -1)
    {
        printf("cannot commit: %d pages, arleady commited %d pages. increase the reserved virtual alloc size.\n",
               num_pages, page_offset);
        ASSERT(ret_code != -1);
    }

    // poison the memory
    asan_poison_memory_region((void *)next_page_base_ptr, new_allocated_size);

    return DB_SUCCESS;
#elif defined(DB_PLATFORM_WINDOWS)
    b8 result = (VirtualAlloc((void *)next_page_base_ptr, new_allocated_size, MEM_COMMIT, PAGE_READWRITE) != NULL);

    if (result == false)
    {
        printf("cannot commit: %d pages, arleady commited %d pages. increase the reserved virtual alloc size.\n",
               num_pages, page_offset);
        ASSERT(result);
    }
    return DB_SUCCESS;
#endif
}

// i dont think i will decomit individual pages, for example for an dynamic array i am pretty sure i will not decommit
// the last page or last 2 pages and so on. so decommit the whole allocated memory size of it.
// decommiting means letting the os reclaim the pages while retaining the virtual address space.
// idk when i will call this though, i think i will just unmap it but oh well :)

db_return_code __db_decommit_virtual_memory(void *memory, size_t size)
{
#if defined(DB_PLATFORM_LINUX) || defined(DB_PLATFORM_MACOS)
    madvise(memory, size, MADV_DONTNEED);
    s32 ret_code = mprotect(memory, size, PROT_NONE);
    ASSERT(ret_code != -1);
    return DB_SUCCESS;
#elif defined(DB_PLATFORM_WINDOWS)
    // size not needed for windows
    VirtualFree(memory, 0, MEM_DECOMMIT);
#endif
    return DB_ERROR;
}
// unmaping the memory
db_return_code __db_release_virtual_memory(void *memory, size_t size)
{
#if defined(DB_PLATFORM_LINUX) || defined(DB_PLATFORM_MACOS)
    s32 ret_code = munmap(memory, size);
    ASSERT(ret_code != -1);
    return DB_SUCCESS;
#elif defined(DB_PLATFORM_WINDOWS)
    VirtualFree(memory, 0, MEM_RELEASE);
    return DB_SUCCESS;
#endif
}

// the easy and the understandable way of doing this is to take an example:
// m = 13 and n = 8, the problem is that we need to align 13 to an multiple of 8.
// or in other words how much do i have to add to 13 in order to make it a multiple of 8.
// really naive solution:
// 1st) find out the interval in which m exists. (k - 1)n <= m <= kn, in our case 8 < 13 < 16
// 2nd) find out how much do we have to add. kn - m. 16 - 13 = 3
// 3rd) add it and then you have the next multiple of n from m.
// there is a problem with this solution. its really inefficeint because first we have to find the largest multiple
// of n
// which is bigger than m. and for large values we might have to do multiple multiplication and then check if that
// number is greater than n. psudo code
//             k = 1
//             while(n * k < m)k++;
//             how_much_to_add = n * k - m
//             m + how_much_to_add == n * k
// so to simplyfy it we do a neat math trick
// we take the mod of the divisor and the quotient for example: m (mod) n
// when we do the mod we get the remainder. the remainder will help us find how much do we have to add to make it an
// multiple of n. for example: 13 mod 8
//       8|13|1
//        - 8      the remainder is the remaning part when an integer cannot divide another interger.
//          5      we can use this fact to find out which integer will divide it.
//    lets visualize this
//       if we wanted to find out the lowest multiple we cound just minus the remainder and be done with it.
//       to find out the next largest integer which can be divided we can do (8 - remainder) + 13
//                          8____13__16
// so this simplifies our solution
//  pseudo code:
//      remainder = m % n
//      padding   = (n - remainder)
//      next_multiple = m + padding
// there is a one small caviat with this solution. lets say m = 8
// we dont want to find the next multiple if m mod n == 0
// so before we add the padding we just mod it my n so that if the remainder was 0 the padding will also be 0
// for example:
// m = 16, n = 8
// remainder = 16 % 8 = 0
// padding = (8 - remainder) = 8 - 0;
// // if we just add the padding we would get 24 which we dont want. so
// aligned_padding = padding % n
// next_multiple = m + aligned_padding
// one liner : ((n - (m % n)) % n) + m
//
// another faster way is to do a bitwise operation if and only if n is a multiple of 2
// this one is a little confusing. i sill dont understand it compeletly
// this works because of the 2'str complement so for example :
//  8: 0000000 00000000 00000000 00001000
//  to get -8 from 8 we do  the following
//  first we have 8'str binary 0000000 00000000 00000000 00001000
//  then we inverse all the bits 0'str map to 1 and 1 maps to 0
//  1111111 11111111 11111111 11110111 then we add  1 to it.
//
//   1111111 11111111 11111111 11110111
// + 0000000 00000000 00000000 00000001
//   1111111 11111111 11111111 11111000  which is -8
//
// -8: 1111111 11111111 11111111 11111000 .this is the 2'str complemenent representation
// which i didnt know even though i have been programming for about 4 years with c :(. damn wtf.....
//
// so how does the following work?
// for example mem = 13 and align to = 8
// 13 in binary      : 00000000 00000000 00000000 00001101
//  8 in binary      : 00000000 00000000 00000000 00001000
// (8 - 1) in binary : 00000000 00000000 00000000 00000111
// 13 + (8 - 1)      : 00000000 00000000 00000000 00010100
// (13 + (8 - 1)) & (-8)
//
// 13 + (8 - 1)      : 00000000 00000000 00000000 00010100
// -8                : 11111111 11111111 11111111 11111000 -> which we derived from the above example
//(13 +(8 - 1)) & -8 : 00000000 00000000 00000000 00010000 -> which would be 16
#define DB_ALIGN_TO_MULTIPLE(mem, align_to) (((uintptr_t)mem + (align_to - 1)) & (-align_to))
#define DB_DEFAULT_MEMORY_ALIGNEMENT 8

// use the macro if its a align_to is the power of 2
uintptr_t __db_align_to_multiple(uintptr_t mem, s32 align_to)
{
    /*
       https://en.wikipedia.org/wiki/data_structure_alignment
      padding = (align - (offset mod align)) mod align
     aligned = offset + padding
     = offset + ((align - (offset mod align)) mod align)
    */
    size_t    padding     = (align_to - (mem % align_to)) % align_to;
    uintptr_t aligned_mem = mem + padding;
    return aligned_mem;
}

//
// args:
// memory_size -> in bytes
db_arena db_arena_init_with_size(db_arena_params *params, size_t memory_size)
{
    db_arena_params def_params = {.type = TYPE_ARENA_LINEAR, .chunk_size = 0};

    if (params)
    {
        def_params.type = params->type;
        if (def_params.type == TYPE_ARENA_CHUNKED)
        {
            ASSERT_WITH_MSG(params->chunk_size, "Chunked Arena specified but Chunk size is 0.");
            def_params.chunk_size = params->chunk_size;
        }
    }

    size_t mem_size = memory_size;
    if (mem_size < DB_ARENA_DEFAULT_COMMITED_MEMORY)
    {
        mem_size = DB_ARENA_DEFAULT_COMMITED_MEMORY;
    }
    else if (mem_size > DB_ARENA_DEFAULT_COMMITED_MEMORY)
    {
        mem_size = DB_ALIGN_TO_MULTIPLE(mem_size, DB_ARENA_DEFAULT_COMMITED_MEMORY);
    }
    // well if you are allocating an arena which has a size greater than db_ARENA_DEFAULT_RESERVED_MEMORY usally
    // 64mb.
    // then why are you allocating it? i cant think of a reason for that :).
    ASSERT(mem_size < DB_ARENA_DEFAULT_RESERVED_MEMORY);

    s32   num_pages_to_commit = mem_size / DB_PAGE_SIZE;
    void *memory              = __db_reserve_virtual_memory(DB_ARENA_DEFAULT_RESERVED_MEMORY);
    ASSERT(memory != NULL);

    db_return_code code = __db_commit_virtual_memory(memory, 0, num_pages_to_commit);
    ASSERT(code != DB_ERROR);

    db_arena arena           = {0};
    arena.curr_page_offset   = num_pages_to_commit;
    arena.allocated_till_now = 0;
    arena.total_size         = mem_size;
    arena.memory             = memory;
    arena.type               = def_params.type;
    arena.curr_mem_pos       = (uintptr_t)arena.memory;

    if (def_params.type == TYPE_ARENA_CHUNKED)
    {
        ASSERT_WITH_MSG(def_params.chunk_size % 8 == 0, "Chunk size must be a multiple of 8 bytes.");
        ASSERT_WITH_MSG(def_params.chunk_size > sizeof(db_arena_chunk_header), "Chunk Size is too small.");
        arena.chunk_size         = def_params.chunk_size;
        arena.allocated_till_now = mem_size; // cause we allocated all of it right
        u64 chunk_count          = mem_size / def_params.chunk_size;

        uintptr_t mem = arena.curr_mem_pos;
        for (u64 i = 0; i < chunk_count; i++)
        {

            /*
               ┌───┐
               │   │ <- previous linked list head
               └───┘

               ┌───┐   ┌───┐
               │   ├<──┤   │ <- new node which was created
               └───┘   └───┘       which will be our new linked list head
                ^
                prev head will be the next node in our linked list;

            */
            db_arena_chunk_header *header = (db_arena_chunk_header *)mem;
            header->next_chunk            = arena.free_list_head;
            arena.free_list_head          = header;

            mem += arena.chunk_size;
            // i think this is unnecessary cause chunk size is supposed to be multiple of 8
            DB_ALIGN_TO_MULTIPLE(mem, DB_DEFAULT_MEMORY_ALIGNEMENT);
        }
    }
    return arena;
}

void db_arena_chunked_init(db_arena *arena, size_t chunk_count, void *mem)
{
    ASSERT((uintptr_t)mem % 8 == 0);
    for (u64 i = 0; i < chunk_count; i++)
    {

        /*
           ┌───┐
           │   │ <- previous linked list head
           └───┘

           ┌───┐   ┌───┐
           │   ├<──┤   │ <- new node which was created
           └───┘   └───┘       which will be our new linked list head
           ^
           prev head will be the next node in our linked list;

*/
        db_arena_chunk_header *header  = (db_arena_chunk_header *)mem;
        header->next_chunk             = arena->free_list_head;
        arena->free_list_head          = header;
        mem                           += arena->chunk_size;
        // i think this is unnecessary cause chunk size is supposed to be multiple of 8
        DB_ALIGN_TO_MULTIPLE(mem, DB_DEFAULT_MEMORY_ALIGNEMENT);
    }
}

void db_arena_resize(db_arena *arena, size_t size)
{
    size_t new_size = 0;
    if (size < DB_PAGE_SIZE)
    {
        new_size = DB_PAGE_SIZE;
    }
    else
    {
        new_size = DB_ALIGN_TO_MULTIPLE(size, DB_PAGE_SIZE);
    }

    s32 num_pages = new_size / DB_PAGE_SIZE;

    // lets say we have commited n - 1 pages of total reserved memory and now we want to allocate n + k number
    // of pages. i am pretty sure that the os will not let us and its going to crash the application.
    db_return_code code = __db_commit_virtual_memory(arena->memory, arena->curr_page_offset, num_pages);

    arena->total_size       += num_pages * DB_PAGE_SIZE;
    arena->curr_page_offset += num_pages;
    ASSERT(code != DB_ERROR);

    if (arena->type == TYPE_ARENA_CHUNKED)
    {
        arena->allocated_till_now = arena->total_size;
        u64 chunk_count           = num_pages / arena->chunk_size;

        //@todo: is this real chat??
        uintptr_t new_page_start_ptr = (uintptr_t)arena->memory + (num_pages * DB_PAGE_SIZE);
        db_arena_chunked_init(arena, chunk_count, (void *)new_page_start_ptr);
        arena->free_list_head = (db_arena_chunk_header *)new_page_start_ptr;
    }
}

// args:
//  arena -> ptr to the db_arena form which you would like to allocate.
//  size ->  memory block size to allocate from the arena.
void *db_arena_alloc(db_arena *arena, size_t size)
{
    ASSERT(arena != NULL);
    ASSERT(size != 0);
    // if the size passed on is bigger than the toal size of the arena then increase the size of the arena to
    // accodomate the allocation.
    size_t aligned_size = DB_ALIGN_TO_MULTIPLE(size, DB_DEFAULT_MEMORY_ALIGNEMENT);
    if (arena->type == TYPE_ARENA_LINEAR)
    {
        if (arena->allocated_till_now + aligned_size > (size_t)arena->total_size)
        {
            db_arena_resize(arena, aligned_size);
        }
    }
    else if (arena->type == TYPE_ARENA_CHUNKED)
    {
        if (arena->free_list_head == NULL) // this means all the free list nodes are occupied
        {
            db_arena_resize(arena, DB_PAGE_SIZE); // @todo: think of a better growth factor for the arena
        }
    }

    void *ret_mem = NULL;
    if (arena->type == TYPE_ARENA_LINEAR)
    {
        // unpoison the memory;
        ret_mem = (void *)arena->curr_mem_pos;
        asan_unpoison_memory_region(ret_mem, aligned_size);

        // align the mem ptr to a multiple of 8 for better cpu/read writes
        uintptr_t temp = arena->curr_mem_pos;

        arena->curr_mem_pos += aligned_size;
        arena->curr_mem_pos  = DB_ALIGN_TO_MULTIPLE(arena->curr_mem_pos, DB_DEFAULT_MEMORY_ALIGNEMENT);

        // sanity check
        // if this triggers then i need to fix the aligned_size logic.
        ASSERT((temp + aligned_size) == arena->curr_mem_pos);

        arena->allocated_till_now += aligned_size;
    }
    else if (arena->type == TYPE_ARENA_CHUNKED)
    {
        db_arena_chunk_header *header = arena->free_list_head;
        ret_mem                       = (void *)((uintptr_t)header + sizeof(db_arena_chunk_header));
        DB_ALIGN_TO_MULTIPLE((uintptr_t)ret_mem, DB_DEFAULT_MEMORY_ALIGNEMENT);

        arena->free_list_head = header->next_chunk;
        header->next_chunk    = NULL; // invalidate the pointer

        // sanity check
        ASSERT((uintptr_t)header + sizeof(db_arena_chunk_header) == (uintptr_t)ret_mem);

        arena->allocated_chunk_count++;
        asan_unpoison_memory_region(ret_mem, arena->chunk_size);
    }
    ASSERT_WITH_MSG(ret_mem, "ARENA ALLOCATION FAILEDD!!!! WHYYY");
    return ret_mem;
}

// hmmmmmmmmmmmmmmmmmmmmmm
/*
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⠞⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠲⡑⢄⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡼⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢮⣣⡀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠱⡀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢣⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⡞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⡆⠀
⠀⠀⠀⠀⠀⠀⠀⠀⡼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⡤⠤⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣹⠀
⠀⠀⠀⠀⢀⣀⣀⣸⢧⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠞⣩⡤⠶⠶⠶⠦⠤⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⡇⡇
⠀⠀⠀⣰⣫⡏⠳⣏⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠚⠁⠀⠀⠀⠀⠀⠀⠙⢿⣿⣶⣄⡀⠀⠀⢀⡀⠀⠀⠀⠀⠀⡀⡅⡇
⠀⠀⢰⡇⣾⡇⠀⠙⣟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⣴⣶⠿⠛⠻⢿⣶⣤⣍⡙⢿⣿⣷⣤⣾⡇⣼⣆⣴⣷⣿⣿⡇⡇
⠀⠀⢸⡀⡿⠁⠀⡇⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⣿⣯⠴⢲⣶⣶⣶⣾⣿⣿⣿⣷⠹⣿⣿⠟⢰⣿⣿⣿⠿⣿⣿⣿⠁
⠀⠀⠈⡇⢷⣾⣿⡿⢱⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠉⠹⣌⠳⣼⣿⣿⣿⣻⣿⣿⣿⣿⡇⠈⠁⢰⣿⣿⣿⣿⣶⣾⣿⣿⠀
⠀⠀⠀⣷⠘⠿⣿⡥⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠃⠌⠉⣿⣿⣿⣿⣿⣿⠟⠃⠀⢀⡿⣿⣿⣿⣿⣿⣿⣿⡞⠀
⠀⠀⠀⢸⡇⠀⠹⠗⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⣿⡿⠟⠉⠉⠀⠀⠀⠈⢃⣿⣿⣿⣿⣿⣿⡻⠀⠀
⠀⠀⠀⠈⢧⠀⠀⠏⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⠁⠀⠀
⠀⠀⠀⠀⠈⢳⠶⠞⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠆⠀⠀⠊⠁⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⠀⠀⠀
⠀⠀⠀⠀⠀⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠂⠀⣼⣿⣀⡰⠀⠀⣤⣄⠀⠀⠀⠀⢹⣿⣿⣿⣿⢻⠀⠀⠀
⠀⠀⠀⠀⠀⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠹⣿⠀⠀⠀⠀⠉⠀⠀⠀⠀⠀⠙⣿⣿⣿⡏⠀⠀⠀
⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣄⢠⣤⣶⣤⣀⠀⢀⣶⣶⣶⣿⣿⠟⠀⠀⠀⠀
⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠖⠁⠀⠀⠀⠀⠀⠻⣿⣿⣥⣤⣯⣥⣾⣿⣿⣿⣿⠋⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣰⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡼⠁⠀⠀⠀⠠⠀⠀⠀⠀⠈⣿⣿⣼⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀
⠀⠀⠀⡰⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠊⠀⠀⠀⣠⣰⣄⡀⠀⢀⣀⣀⣛⣟⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀⠀⠀
⠀⣠⠜⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣼⠾⠛⠛⠻⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀
⠾⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡟⠀⠀⠀⠀⠠⣄⣉⣉⣻⣿⣿⣿⣿⣿⣿⡟⠧⢄⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⠅⠀⠀⠀⠀⠘⠉⠹⣿⣿⣿⣿⣿⣿⣿⣿⣧⡀⠀⠉⠓⠢⣄⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣉⣿⣿⣿⣿⣿⣿⣿⣿⣷⣻⡄⠀⠀⢀⡑⠢⠄
*/
db_return_code db_arena_free_node(db_arena *arena, void *ptr)
{
    ASSERT(ptr);
    ASSERT(arena);
    ASSERT(arena->type == TYPE_ARENA_CHUNKED);

    db_arena_chunk_header *header = (db_arena_chunk_header *)((uintptr_t)ptr - sizeof(db_arena_chunk_header));
    memset(header, 0, arena->chunk_size);
    // check for ptr validity
    // kinda useless??
    {

        u64 chunk_count = arena->total_size / arena->chunk_size;

        db_arena_chunk_header *check = arena->memory;
        b8                     found = false;
        for (u64 i = 0; i < chunk_count && !found; i++)
        {
            if (check == header)
            {
                found = true;
            }
            check = (db_arena_chunk_header *)((uintptr_t)check + arena->chunk_size);
        }
        ASSERT_WITH_MSG(found, "This ptr is not valid.");
    }
    header->next_chunk    = arena->free_list_head;
    arena->free_list_head = header;

    return DB_SUCCESS;
}

db_return_code db_arena_reset(db_arena *arena)
{
    ASSERT(arena != NULL);

    memset(arena->memory, 0, arena->allocated_till_now);
    arena->allocated_till_now = 0;
    arena->curr_mem_pos       = (uintptr_t)arena->memory;
    arena->free_list_head     = NULL;

    return DB_SUCCESS;
}

db_return_code db_arena_free(db_arena *arena)
{
    ASSERT(arena != NULL);

    // there might be the case that we allocated/commited more than db_ARENA_DEFAULT_RESERVED_MEMORY.
    size_t total_reserved_size = db_max(arena->total_size, DB_ARENA_DEFAULT_RESERVED_MEMORY);

    db_return_code res = __db_release_virtual_memory(arena->memory, total_reserved_size);
    ASSERT(res != DB_ERROR);

    return DB_SUCCESS;
}

db_return_code __db_array_resize(db_array_skeleton *array)
{
    ASSERT(array != NULL);

    size_t new_size = array->total_length * DB_ARRAY_DEFAULT_RESIZE_FACTOR;
    void  *new_mem  = db_arena_alloc(array->arena, new_size * array->type_size);
    memcpy(new_mem, array->data, array->length * array->type_size);
    array->data          = new_mem;
    array->total_length += new_size;
    return DB_SUCCESS;
}

db_return_code __db_array_init(db_arena *shared_arena, db_array_skeleton *array, size_t type_size)
{

    // maybe a bit wasteful but we've already commited this much isnt it?
    size_t array_size = DB_ARRAY_DEFAULT_ALLOCATION_BUCKETS * type_size;
    array_size        = DB_ALIGN_TO_MULTIPLE(array_size, DB_DEFAULT_MEMORY_ALIGNEMENT);

    void *memory = db_arena_alloc(shared_arena, array_size);

    array->total_length = array_size / type_size;
    array->length       = 0;
    array->type_size    = type_size;
    array->arena        = shared_arena;
    array->data         = memory;
    return DB_SUCCESS;
}

void __db_array_free(db_array_skeleton *array)
{
    ASSERT(array != NULL);
    array->total_length = 0;
    array->length       = 0;
    array->type_size    = 0;
    array->data         = NULL;
    // does absolutely nothing actually
    // print warning
    printf("WHY ARE YOU MANNUALLY FREEING UP MEMORY???????\n");
    ASSERT_WITH_MSG(false, "I DID THIS JUST TO PISS YOU OFFF");
}

db_return_code __db_stack_init(db_arena *shared_arena, db_stack_skeleton *array, size_t type_size)
{
    // maybe a bit wasteful but we've already commited this much isnt it?
    size_t stack_size = DB_ARRAY_DEFAULT_ALLOCATION_BUCKETS * type_size;
    if (stack_size >= DB_PAGE_SIZE)
    {
        printf("The stack you are initializing is already bigger than the page size on your os. You might consider "
               "using the big array for this one cheif.\n");
    }
    stack_size = DB_ALIGN_TO_MULTIPLE(stack_size, DB_DEFAULT_MEMORY_ALIGNEMENT);

    void *memory = db_arena_alloc(shared_arena, stack_size);

    array->total_length = stack_size / type_size;
    array->length       = 0;
    array->type_size    = type_size;
    array->arena        = shared_arena;
    array->data         = memory;
    return DB_SUCCESS;
}

db_return_code __db_stack_resize(db_stack_skeleton *stack)
{
    ASSERT(stack != NULL);

    size_t new_size = stack->total_length * DB_ARRAY_DEFAULT_RESIZE_FACTOR;
    void  *new_mem  = db_arena_alloc(stack->arena, new_size * stack->type_size);
    memcpy(new_mem, stack->data, stack->length * stack->type_size);
    stack->data          = new_mem;
    stack->total_length += new_size;
    return DB_SUCCESS;
}

void __db_stack_free(db_stack_skeleton *stack)
{
    ASSERT(stack != NULL);
    stack->total_length = 0;
    stack->length       = 0;
    stack->type_size    = 0;
    stack->data         = NULL;
    // does absolutely nothing actually
    // print warning
    printf("WHY ARE YOU MANNUALLY FREEING UP MEMORY???????\n");
    ASSERT_WITH_MSG(false, "I DID THIS JUST TO PISS YOU OFFF");
}

#define DB_SIZE_T_BITS ((sizeof(size_t)) * 8)

#define DB_rotate_left(val, n) (((val) << (n)) | ((val) >> (db_SIZE_T_BITS - (n))))
#define DB_rotate_right(val, n) (((val) >> (n)) | ((val) << (db_SIZE_T_BITS - (n))))

u64 db_murmur64A_seed(void const *const key, u64 len, u64 seed)
{
    const u64 m = 0xc6a4a7935bd1e995LLU;
    const int r = 47;

    u64 h = seed ^ (len * m);

    const u64 *data = (const u64 *)key;
    const u64 *end  = (len >> 3) + data;

    while (data != end)
    {
        u64 k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char *)data;

    switch (len & 7)
    {
        case 7:
            h ^= (u64)(data2[6]) << 48;
        case 6:
            h ^= (u64)(data2[5]) << 40;
        case 5:
            h ^= (u64)(data2[4]) << 32;
        case 4:
            h ^= (u64)(data2[3]) << 24;
        case 3:
            h ^= (u64)(data2[2]) << 16;
        case 2:
            h ^= (u64)(data2[1]) << 8;
        case 1:
            h ^= (u64)(data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

char db_char_to_lower(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        char ret = 'a' + (c - 'A');
        return ret;
    }
    return c;
}
char db_char_to_upper(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        char ret = 'A' + (c - 'a');
        return ret;
    }
    return c;
}
b8 db_char_is_space(char c)
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v')
    {
        return true;
    }
    return false;
}
b8 db_char_is_digit(char c)
{
    if (c >= '0' && c <= '9')
    {
        return true;
    }
    return false;
}
b8 db_char_is_hex_digit(char c)
{
    if (db_char_is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
    {
        return true;
    }
    return false;
}
b8 db_char_is_alphabet(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    {
        return true;
    }
    return false;
}
b8 db_char_is_alphanumeric(char c)
{
    if (db_char_is_alphabet(c) || db_char_is_digit(c))
    {
        return true;
    }
    return false;
}
// hex digit
// IF YOU ARE TURNING A HEX INTO A DIGIT THEN:
// if its a it will return 10 and so on till f == 15 but if the value is anything plus f, so g,h... it will return
// an incorecct value so always call is db_char_is_hex_digit() before calling this.
s32 db_digit_to_int(char c)
{
    if (db_char_is_digit(c))
    {
        return (c - '0');
    }
    return (c - 'W');
}
s32 db_hex_digit_to_int(char c)
{
    if (db_char_is_digit(c))
    {
        return db_digit_to_int(c);
    }
    else if (c <= 'a' && c <= 'f')
    {
        return 10 + c;
    }
    else if (c <= 'A' && c <= 'F')
    {
        return 10 + c;
    }
    return -1;
}

void db_str_to_lower(char *str)
{
    // well if you forget the null terminator then we're kind of fucked
    while (*str)
    {
        *str = db_char_to_lower(*str);
        str++;
    }
}
void db_str_to_upper(char *str)
{
    // well if you forget the null terminator then we're kind of fucked
    while (*str)
    {
        *str = db_char_to_upper(*str);
        str++;
    }
}

char const *db_char_first_occurence(char const *str, char c)
{
    char ch = c;
    for (; *str != ch; str++)
    {
        if (*str == '\0')
        {
            return NULL;
        }
    }
    return str;
}
char const *db_char_last_occurence(char const *str, char c)
{
    char const *result = NULL;
    do
    {
        if (*str == c)
        {
            result = str;
        }
    } while (*str++);

    return result;
}
// this is blashemous
size_t db_strlen(char const *str)
{
    return strlen(str);
}

// this is blashemous
s32 db_strcmp(char const *s1, char const *s2)
{
    int ans = strcmp(s1, s2);
    return ans == 0;
}

db_string db_string_make(db_arena *arena, char const *a)
{
    db_string str = {.arena = arena, .length = 0, .data = NULL};
    s32       len = strlen(a);

    if (arena->type == TYPE_ARENA_CHUNKED)
    {
        s64 useful_chunk_payload_size = arena->chunk_size - (sizeof(db_arena_chunk_header) + 1);
        // f32 chunk_count               = ceil((f32)len / useful_chunk_payload_size);

        str.data = db_arena_alloc(arena, arena->chunk_size);

        const char *orig  = a;
        char       *start = str.data;
        char       *b     = str.data;

        while (*orig)
        {
            *b = *orig;
            orig++;
            b++;
            str.length++;
            // -1 because the length that we can write to is arena->chunk_size - 1. Just like in array length
            // this is cause its 0 -based indexeing
            if ((uintptr_t)b - (uintptr_t)start == (arena->chunk_size - sizeof(db_arena_chunk_header) - 1))
            {
                db_arena_chunk_header *header = DB_ARENA_CHUNK_HEADER(start);
                ASSERT(header->next_chunk == NULL);
                b                  = db_arena_alloc(arena, arena->chunk_size);
                start              = b;
                header->next_chunk = (db_arena_chunk_header *)((uintptr_t)b - sizeof(db_arena_chunk_header));
            }
        }
        s64 length = len;
        // hmmm it will be zero there too but lets just me more explicit
        *b         = '\0';
        str.length = length;
    }
    else
    {
        str.data     = db_arena_alloc(arena, DB_STRING_LINEAR_SIZE);
        str.capacity = DB_STRING_LINEAR_SIZE;

        const char *orig  = a;
        char       *start = str.data;
        char       *b     = str.data;

        while (*orig)
        {
            *b = *orig;
            b++;
            orig++;
            str.length++;
        }
        // just to be sure
        start[str.length] = '\0';
    }

    return str;
}

char *db_string_get_cstr(db_arena *arena, db_string const *const str)
{
    ASSERT_WITH_MSG(arena->type == TYPE_ARENA_LINEAR, "Cannot pass a cstr with a chunked arena");
    char *cpy = NULL;
    if (str->length == 0)
        return "";
    if (str->arena->type == TYPE_ARENA_CHUNKED)
    {
        cpy         = db_arena_alloc(arena, str->length);
        char *c     = cpy;
        char *start = str->data;
        char *b     = str->data;

        while (*b)
        {
            *c = *b;
            c++;
            b++;

            // -1 because the length that we can write to is arena->chunk_size - 1. Just like in array length
            // this is cause its 0 -based indexeing
            if ((uintptr_t)b - (uintptr_t)start == (str->arena->chunk_size - sizeof(db_arena_chunk_header) - 1))
            {
                db_arena_chunk_header *header = DB_ARENA_CHUNK_HEADER(start);
                if (header->next_chunk)
                {
                    start = (char *)((uintptr_t)header->next_chunk + sizeof(db_arena_chunk_header));
                    b     = start;
                }
            }
        }
    }
    else
    {
        cpy = str->data;
    }
    return cpy;
}

// hmmm what if we had already allocated n chunks before
void db_string_make_reserve(db_string str, s64 capacity)
{
    if (str.arena->chunk_size > (u64)capacity)
    {
        return;
    }
    else
    {
        // @todo:
        // u32 chunk_count = capacity / str.arena->chunk_size;
    }
    printf("Not implemented db_string_make_reserve yet. If you're reading this then why tf havent you implemeneted it "
           "till now??\n");
}
void db_string_free(db_string *const str)
{
    // does absolutley nothing.
    printf("this call to string free does nothing. Why are you using it anyways??\n");
}
db_string db_string_duplicate(db_arena *arena, db_string const *const str)
{
    db_string new_str = db_string_make(arena, db_string_get_cstr(arena, str));
    return new_str;
}
s64 db_string_length(db_string const *const str)
{
    return str->length; // well this is useless
}
void db_string_clear(db_string *const str)
{
    // clear all the chunks
    if (str->arena->type == TYPE_ARENA_CHUNKED)
    {
        db_arena_chunk_header *header = DB_ARENA_CHUNK_HEADER(str->data);
        while (header->next_chunk)
        {
            db_arena_chunk_header *prev = header;
            header                      = header->next_chunk;
            db_arena_free_node(str->arena, prev);
        }
        str->data = NULL;
    }
    else
    {
        memset(str->data, 0, str->length);
        str->length = 0;
    }
}

void db_string_append(db_string *const str, const char *other)
{
    s32 len = strlen(other);

    if (str->arena->type == TYPE_ARENA_CHUNKED)
    {
        db_arena_chunk_header *header = (db_arena_chunk_header *)(str->data - sizeof(db_arena_chunk_header));
        while (header->next_chunk)
        {
            header = (db_arena_chunk_header *)(uintptr_t)header + str->arena->chunk_size;
        }
        const char *orig  = other;
        char       *start = str->data;
        char       *b     = str->data;

        while (*b)
        {
            b++;
        }

        while (*orig)
        {
            *b = *orig;
            orig++;
            b++;
            str->length++;
            // -1 because the length that we can write to is arena->chunk_size - 1. Just like in array length
            // this is cause its 0 -based indexeing
            if ((uintptr_t)b - (uintptr_t)start == (str->arena->chunk_size - sizeof(db_arena_chunk_header) - 1))
            {
                db_arena_chunk_header *header = DB_ARENA_CHUNK_HEADER(start);
                b                             = db_arena_alloc(str->arena, str->arena->chunk_size);
                start                         = b;

                header->next_chunk = (db_arena_chunk_header *)(b - sizeof(db_arena_chunk_header));
            }
        }
    }
    else
    {
        const char *orig  = other;
        char       *start = str->data;
        char       *b     = str->data;

        while (*b)
        {
            b++;
        }
        if (str->length + 1 >= str->capacity)
        {
            char *new_data = db_arena_alloc(str->arena, DB_ARRAY_DEFAULT_RESIZE_FACTOR * str->capacity);
            memcpy(new_data, str->data, str->length);
            str->data = new_data;

            b     = str->data;
            start = str->data;
        }

        while (*orig)
        {
            *b = *orig;
            orig++;
            b++;
            str->length++;
        }
    }
}

void db_string_append_char(db_string *const str, const char c)
{

    if (str->arena->type == TYPE_ARENA_CHUNKED)
    {
        db_arena_chunk_header *header = (db_arena_chunk_header *)(str->data - sizeof(db_arena_chunk_header));
        while (header->next_chunk)
        {
            header = (db_arena_chunk_header *)(uintptr_t)header + str->arena->chunk_size;
        }
        char *start = str->data;
        char *b     = str->data;

        while (*b)
        {
            b++;
        }

        if ((uintptr_t)b - (uintptr_t)start == (str->arena->chunk_size - sizeof(db_arena_chunk_header) - 1))
        {
            db_arena_chunk_header *header = DB_ARENA_CHUNK_HEADER(start);
            b                             = db_arena_alloc(str->arena, str->arena->chunk_size);
            start                         = b;

            header->next_chunk = (db_arena_chunk_header *)(b - sizeof(db_arena_chunk_header));
        }
        *b = c;
    }
    else
    {
        char *start = str->data;
        char *b     = str->data;

        while (*b)
        {
            b++;
        }
        if (str->length + 1 >= str->capacity)
        {
            char *new_data = db_arena_alloc(str->arena, DB_ARRAY_DEFAULT_RESIZE_FACTOR * str->capacity);
            memcpy(new_data, str->data, str->length);
            str->data = new_data;

            b     = str->data;
            start = str->data;
        }

        *b = c;
        b++;
        str->length++;
        start[str->length] = '\0';
    }
}

// well this looks like for utf8 strings. Well Let me figure that out later
// db_string db_string_append_rune(db_string str, Rune r);
// db_string db_string_append_fmt(db_string str, char const *fmt, ...)
//{
//    // hmm this is gonna take a bit of time.  I have to make my own (va args) format parser
//    return 0;
//}

void db_string_set(db_string *const str, char const *cstr)
{
    db_string_clear(str);
    db_string_append(str, cstr);
}
// hmmmmm I dont think we need this because our arena will automatically scale based on appending
// void db_string_make_space_for(db_string str, s64 add_len)
//{
//}

// s64 db_string_allocation_size(db_string const str)
//{
// }

b8 db_strings_are_equal(db_string const *lhs, db_string const *rhs)
{
    u64 lhs_count = lhs->length;
    u64 rhs_count = rhs->length;

    if (lhs_count != rhs_count)
        return false;

    char       *a       = lhs->data;
    char       *b       = rhs->data;
    const char *a_start = a;
    const char *b_start = b;
    ASSERT(lhs->arena->type == TYPE_ARENA_CHUNKED && rhs->arena->type == TYPE_ARENA_CHUNKED);
    while (*a && *b)
    {
        // -1 because the length that we can write to is arena->chunk_size - 1. Just like in array length
        // this is cause its 0 -based indexeing
        if (*a != *b)
        {
            return false;
        }
        a++;
        b++;
        if ((uintptr_t)a - (uintptr_t)a_start == (lhs->arena->chunk_size - sizeof(db_arena_chunk_header) - 1) &&
            (uintptr_t)b - (uintptr_t)b_start == (lhs->arena->chunk_size - sizeof(db_arena_chunk_header) - 1))
        {
            db_arena_chunk_header *a_header = DB_ARENA_CHUNK_HEADER(a);
            a_header                        = a_header->next_chunk;
            a                               = (char *)((uintptr_t)a_header->next_chunk + sizeof(db_arena_chunk_header));
            a_start                         = a;

            db_arena_chunk_header *b_header = DB_ARENA_CHUNK_HEADER(b);
            b_header                        = b_header->next_chunk;
            b                               = (char *)((uintptr_t)b_header->next_chunk + sizeof(db_arena_chunk_header));
            b_start                         = b;
        }
    }
    return true;
}
//@fix: this is incomplete
// db_string db_string_trim(db_arena *scratch_arena, db_string *const str, char const *cut_set)
// {
//     u64 length = str->length;
//
//     db_arena_chunk_header *start_header = DB_ARENA_CHUNK_HEADER(str->data);
//
//     char *final_start_pos = NULL;
//     char *final_end_pos   = NULL;
//
//     while (start_header)
//     {
//         char *start_pos     = (char *)((uintptr_t)start_header + sizeof(db_arena_chunk_header));
//         char *end_pos_chunk = start_pos + (str->arena->chunk_size - (sizeof(db_arena_chunk_header) + 1));
//         while (start_pos <= end_pos_chunk && db_char_first_occurence(cut_set, *start_pos))
//         {
//             start_pos++;
//         }
//         if (start_pos != (end_pos_chunk + 1))
//         {
//             final_start_pos = start_pos;
//             break;
//         }
//         start_header = start_header->next_chunk;
//     }
//     // go to the last node
//
//     db_arena_chunk_header *end_chunk   = DB_ARENA_CHUNK_HEADER(str->data);
//     s64                    chunk_count = 0;
//     while (end_chunk->next_chunk)
//     {
//         end_chunk = end_chunk->next_chunk;
//         chunk_count++;
//     }
//     // store in an array
//     db_arena_chunk_header **nodes = db_arena_alloc(scratch_arena, sizeof(db_arena_chunk_header *) * chunk_count);
//     end_chunk                     = DB_ARENA_CHUNK_HEADER(str->data);
//     s32 i                         = 0;
//     while (end_chunk)
//     {
//         nodes[i++] = end_chunk;
//         end_chunk  = end_chunk->next_chunk;
//     }
//     db_arena_chunk_header *start_chunk = DB_ARENA_CHUNK_HEADER(str->data);
//     end_chunk                          = nodes[--i];
//
//     while (i >= 0 && end_chunk)
//     {
//         char *start_pos = (char *)((uintptr_t)end_chunk + sizeof(db_arena_chunk_header));
//         char *end_pos   = start_pos + (str->arena->chunk_size - (sizeof(db_arena_chunk_header) + 1));
//         while (end_pos >= start_pos && db_char_first_occurence(cut_set, *end_pos))
//         {
//             end_pos--;
//         }
//         if (end_pos != (start_pos - 1))
//         {
//             final_end_pos = end_pos;
//             break;
//         }
//         end_chunk = nodes[--i];
//     }
//     ASSERT(final_start_pos);
//     ASSERT(final_end_pos);
//
//     db_string new_str = db_string_make(str->arena, "");
//     while (final_start_pos != final_end_pos)
//     {
//         if (*final_start_pos == '\0')
//         {
//             start_header = start_header->next_chunk;
//             ASSERT(start_chunk); // should never happen
//             final_start_pos = (char *)((uintptr_t)start_header + sizeof(db_arena_chunk_header));
//             if (final_start_pos == final_end_pos)
//             {
//                 db_string_append_char(&new_str, *final_start_pos);
//                 new_str.length++;
//                 break;
//             }
//         }
//         db_string_append_char(&new_str, *final_start_pos);
//         final_start_pos++;
//         new_str.length++;
//     }
//     return new_str;
// }
/**
 * WHITESPACE REFERENCE:
 * ' ' Space
 * \t  Horizontal Tab
 * \r  Carriage Return (Line start)
 * \n  Line Feed (New line)
 * \v  Vertical Tab
 * \f  Form Feed (Page break) for printers
 */
// db_string db_string_trim_space(db_arena *scratch_arena, db_string *const str) // Whitespace ` \t\r\n\v\f`
// {
//     return db_string_trim(scratch_arena, str, " \t\r\n\v\f");
// } // Whitespace ` \t\r\n\v\f`

FILE *db_file_open(char const *file_path, db_file_mode mode)
{
#ifdef DB_PLATFORM_LINUX
    char const *f_os_mode = NULL;

    switch (mode & db_file_mode_modes)
    {
        case db_file_mode_read:
            f_os_mode = "r";
            break;
        case db_file_mode_write:
            f_os_mode = "w";
            break;
        case db_file_mode_append:
            f_os_mode = "a";
            break;
        case db_file_mode_read | db_file_mode_rw:
            f_os_mode = "r+";
            break;
        case db_file_mode_write | db_file_mode_rw:
            f_os_mode = "w+";
            break;
        case db_file_mode_append | db_file_mode_rw:
            f_os_mode = "a+";
            break;
        default:
            printf("Invalid file mode\n");
            return NULL;
    }

    FILE *f = fopen(file_path, f_os_mode);
    if (!f)
    {
        perror("db_file_open failed");
    }
    return f;
#else
    return NULL;
#endif
}

db_file_contents db_file_read_contents(db_arena *a, db_file_mode mode, b32 zero_terminate, char const *filepath)
{
    db_file_contents contents = {};

    FILE *f = db_file_open(filepath, mode);
    if (f == NULL)
    {
        return contents;
    }
    if (fseek(f, 0L, SEEK_END) != 0)
    {
        fclose(f);
        return contents; // Seek failed
    }

    contents.size = ftell(f);
    rewind(f);

    contents.data     = db_arena_alloc(a, contents.size + 1);
    s_size bytes_read = fread(contents.data, sizeof(char), contents.size, f);

    if (bytes_read != contents.size)
    {
        printf("Error reading bytes out of %s\n", filepath);
    }
    fclose(f);
    if (zero_terminate)
    {
        ((char *)contents.data)[contents.size] = '\0';
    }
    return contents;
}

#endif
