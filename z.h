#ifndef Z_H
#define Z_H

#ifndef ZAPI
#define ZAPI
#endif

#ifndef Z_ASSERT
#define Z_ASSERT assert
#endif

#ifndef Z_REALLOC
#define Z_REALLOC realloc
#endif

#ifndef Z_MALLOC
#define Z_MALLOC malloc
#endif

#ifndef Z_FREE
#define Z_FREE free
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>

#define Z_ARRAY_INITIALIZER NULL

#ifndef Z_ARRAY_INIT_CAPACITY
#define Z_ARRAY_INIT_CAPACITY 128
#endif

typedef struct {
    size_t count;
    size_t capacity;
} z_array_header_t;

#define z_arrhdr(a_)   ((z_array_header_t*)((char*)(a_)-sizeof(z_array_header_t)))
#define z_arrcnt(a_)   ((a_) ? z_arrhdr(a_)->count : 0)
#define z_arrcap(a_)   ((a_) ? z_arrhdr(a_)->capacity : 0)
#define z_arrbegin(a_) (a_)
#define z_arrend(a_)   ((a_) ? (a_) + z_arrcnt(a_) : NULL)

#define z_arr_grow(a_) do {                                                        \
    z_array_header_t *h;                                                           \
    size_t old_cnt, old_cap, new_cap;                                              \
    h = (a_) ? z_arrhdr(a_) : NULL;                                                \
    old_cnt = z_arrcnt(a_);                                                        \
    old_cap = z_arrcap(a_);                                                        \
    new_cap = old_cap < Z_ARRAY_INIT_CAPACITY ? Z_ARRAY_INIT_CAPACITY : 2*old_cap; \
    h = Z_REALLOC(h, sizeof(*h)+new_cap*sizeof(*(a_)));                            \
    Z_ASSERT(h && "run out of memory");                                            \
    h->count = old_cnt;                                                            \
    h->capacity = new_cap;                                                         \
    (a_) = (void*)(h + 1);                                                         \
} while (0)
#define z_arr_push(a_, item_) do {         \
    if (z_arrcnt(a_) + 1 > z_arrcap(a_)) { \
        z_arr_grow(a_);                    \
    }                                      \
    (a_)[z_arrhdr(a_)->count++] = (item_); \
} while (0)
#define z_arr_pop(a_) (Z_ASSERT(z_arrcnt(a_) > 0 && "array underflow"), (a_)[--z_arrhdr(a_)->count])
#define z_arr_foreach(a_, iter_) for (iter_ = z_arrbegin(a_); iter_ < z_arrend(a_); iter_++)
#define z_arr_clear(a_) do { if (a_) z_arrhdr(a_)->count = 0; } while (0)
#define z_arr_free(a_) do { if (a_) Z_FREE(z_arrhdr(a_)); (a_) = NULL; } while (0)

#define z_wrapinc(x_, bound_)               (((x_) + 1 >= (bound_)) ? 0 : (x_) + 1)
#define z_wrapdec(x_, bound_)               (((x_) == 0) ? ((bound_) - 1) : (x_) - 1)
#define z_clamp(x_, lo_, hi_)               ((x_) < (lo_) ? (lo_) : ((x_) > (hi_) ? (hi_) : (x_)))
#define z_offset_of(T, member_)             ((size_t)&(((T*)0)->member_))
#define z_container_of(ptr_, T, member_)    ((T*)((char*)(ptr_) - z_offset_of(T, member_)))
#define z_align_up(x_, align_)              (((x_) + (align_) - 1) & ~((align_) - 1))
#define z_align_down(x_, align_)            ((x_) & ~((align_) - 1))

#ifndef Z_TEMP_BUFFER_CAPACITY
#define Z_TEMP_BUFFER_CAPACITY (2 << 10)
#endif

ZAPI void  *z_ta_alloc(size_t n);
ZAPI size_t z_ta_save(void);
ZAPI void  *z_ta_restore(size_t checkpoint);
ZAPI char  *z_ta_strdup(const char *s);
ZAPI char  *z_ta_strndup(const char *s, size_t n);
ZAPI char  *z_ta_sprintf(const char *fmt, ...);
ZAPI char  *z_ta_snprintf(size_t n, const char *fmt, ...);
ZAPI char  *z_ta_vsprintf(const char *fmt, va_list ap);
ZAPI char  *z_ta_vsnprintf(size_t n, const char *fmt, va_list ap);

typedef struct {
    size_t count;
    const char *data;
} z_string_view_t;

#define z_svfmt "%.*s"
#define z_svarg(sv) (sv).count, (sv).data

ZAPI z_string_view_t z_sv_from_cstr(const char *cstr);
ZAPI z_string_view_t z_sv_from_parts(const char *data, size_t count);
ZAPI z_string_view_t z_sv_substr(z_string_view_t sv, size_t start, size_t count);
ZAPI z_string_view_t z_sv_trim_left(z_string_view_t sv);
ZAPI z_string_view_t z_sv_trim_right(z_string_view_t sv);
ZAPI z_string_view_t z_sv_trim(z_string_view_t sv);
ZAPI bool            z_sv_starts_with(z_string_view_t sv, z_string_view_t prefix);
ZAPI bool            z_sv_ends_with(z_string_view_t sv, z_string_view_t suffix);
ZAPI bool            z_sv_equal(z_string_view_t a, z_string_view_t b);

typedef struct {
    size_t count;
    size_t capacity;
    char *chars;
} z_string_builder_t;

ZAPI z_string_view_t z_sb_view(z_string_builder_t *sb);
ZAPI void            z_sb_free(z_string_builder_t *sb);
ZAPI void            z_sb_clear(z_string_builder_t *sb);
ZAPI bool            z_sb_reserve(z_string_builder_t *sb, size_t need);
ZAPI void            z_sb_append_char(z_string_builder_t *sb, char c);
ZAPI void            z_sb_append_sv(z_string_builder_t *sb, z_string_view_t sv);
ZAPI void            z_sb_append_cstr(z_string_builder_t *sb, const char *cstr);
ZAPI void            z_sb_append_parts(z_string_builder_t *sb, const char *data, size_t n);

typedef struct {
    size_t count;
    size_t capacity;
    size_t head;
} z_deque_header_t;

#ifndef Z_DEQUE_INIT_CAPACITY
#define Z_DEQUE_INIT_CAPACITY 128
#endif

#define Z_DEQUE_INITIALIZER NULL

#define z_deqhdr(d_)   ((z_deque_header_t*)((char*)(d_)-sizeof(z_deque_header_t)))
#define z_deqcnt(d_)   ((d_) ? z_deqhdr(d_)->count : 0)
#define z_deqcap(d_)   ((d_) ? z_deqhdr(d_)->capacity : 0)
#define z_deqbegin(d_) ((d_) ? (d_) + z_deqhdr(d_)->head : NULL)
#define z_deqend(d_)   ((d_) ? (d_) + (z_deqhdr(d_)->head + z_deqcnt(d_)) % z_deqcap(d_) : NULL)
#define z_deqhead(d_)  (Z_ASSERT(z_deqcnt(d_) > 0 && "deque is empty"), (d_)[z_deqhdr(d_)->head])
#define z_deqtail(d_)  (Z_ASSERT(z_deqcnt(d_) > 0 && "deque is empty"), \
        (d_)[(z_deqhdr(d_)->head + z_deqcnt(d_) - 1) % z_deqcap(d_)])

#define z_deq_grow(d_) do {                                                         \
    z_deque_header_t *h;                                                            \
    size_t old_cnt, old_cap, old_head, new_cap;                                     \
    h = (d_) ? z_deqhdr(d_) : NULL;                                                 \
    old_cnt  = z_deqcnt(d_);                                                        \
    old_cap  = z_deqcap(d_);                                                        \
    old_head = h ? h->head : 0;                                                     \
    new_cap  = old_cap < Z_DEQUE_INIT_CAPACITY ? Z_DEQUE_INIT_CAPACITY : 2*old_cap; \
    h = Z_MALLOC(sizeof(*h)+new_cap*sizeof(*(d_)));                                 \
    Z_ASSERT(h && "run out of memory");                                             \
    memset(h, 0, sizeof(*h)+new_cap*sizeof(*(d_)));                                 \
    if (d_) {                                                                       \
        char *p;                                                                    \
        size_t offset;                                                              \
        p = (char*)(h + 1);                                                         \
        offset = 0;                                                                 \
        memmove(p+offset, (d_)+old_head, (old_cap-old_head)*sizeof(*(d_)));         \
        offset = (old_cap-old_head)*sizeof(*(d_));                                  \
        memmove(p+offset, (d_), old_head*sizeof(*(d_)));                            \
    }                                                                               \
    h->count    = old_cnt;                                                          \
    h->capacity = new_cap;                                                          \
    h->head     = 0;                                                                \
    z_deq_free(d_);                                                                 \
    (d_) = (void*)(h + 1);                                                          \
} while (0)
#define z_deq_push_back(d_, item_) do {                 \
    z_deque_header_t *h;                                \
    if (z_deqcnt(d_) + 1 > z_deqcap(d_)) {              \
        z_deq_grow(d_);                                 \
    }                                                   \
    h = z_deqhdr(d_);                                   \
    (d_)[(h->head + h->count) % h->capacity] = (item_); \
    h->count++;                                         \
} while (0)
#define z_deq_push_front(d_, item_) do {                 \
    z_deque_header_t *h;                                 \
    if (z_deqcnt(d_) + 1 > z_deqcap(d_)) {               \
        z_deq_grow(d_);                                  \
    }                                                    \
    h = z_deqhdr(d_);                                    \
    h->head = (h->head + h->capacity - 1) % h->capacity; \
    (d_)[h->head] = (item_);                             \
    h->count++;                                          \
} while (0)
#define z_deq_pop_back(d_) (Z_ASSERT(z_deqcnt(d_) > 0 && "deque underflow"), \
        z_deqhdr(d_)->count--,                                               \
        (d_)[(z_deqhdr(d_)->head + z_deqhdr(d_)->count) % z_deqhdr(d_)->capacity])
#define z_deq_pop_front(d_) (Z_ASSERT(z_deqcnt(d_) > 0 && "deque underflow"),   \
        z_deqhdr(d_)->count--,                                                  \
        z_deqhdr(d_)->head = (z_deqhdr(d_)->head + 1) % z_deqhdr(d_)->capacity, \
        (d_)[(z_deqhdr(d_)->head + z_deqhdr(d_)->capacity - 1) % z_deqhdr(d_)->capacity])
#define z_deq_clear(d_) do { if (d_) z_deqhdr(d_)->head = z_deqhdr(d_)->count = 0; } while (0)
#define z_deq_free(d_) do { if (d_) Z_FREE(z_deqhdr(d_)); (d_) = NULL; } while (0)
#define z_deq_foreach(d_, iter_) for (iter_ = z_deqbegin(d_); iter_ != z_deqend(d_); \
        iter_ = (iter_ + 1) == (d_) + z_deqcap(d_) ? (d_) : (iter_ + 1))

#endif /* Z_H */

#ifdef Z_IMPLEMENTATION

static char z__temp_buffer[Z_TEMP_BUFFER_CAPACITY];
static size_t z__temp_used;

ZAPI void *z_ta_alloc(size_t n)
{
    size_t remain;
    void *p;

    remain = Z_TEMP_BUFFER_CAPACITY - z__temp_used;
    if (remain < n) return NULL;
    p = &z__temp_buffer[z__temp_used];
    z__temp_used += n;
    return p;
}

ZAPI size_t z_ta_save(void)
{
    return z__temp_used;
}

ZAPI void *z_ta_restore(size_t checkpoint)
{
    z__temp_used = checkpoint;
}

ZAPI char *z_ta_strdup(const char *s)
{
    size_t len;
    char *p;

    len = strlen(s);
    p = z_ta_alloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

ZAPI char *z_ta_strndup(const char *s, size_t n)
{
    size_t len;
    char *p;

    len = strnlen(s, n);
    p = z_ta_alloc(len + 1);
    if (p) {
        memcpy(p, s, len);
        p[len] = '\0';
    }
    return p;
}

ZAPI char *z_ta_sprintf(const char *fmt, ...)
{
    va_list ap;
    char *p;

    va_start(ap, fmt);
    p = z_ta_vsprintf(fmt, ap);
    va_end(ap);
    return p;
}

ZAPI char *z_ta_snprintf(size_t n, const char *fmt, ...)
{
    va_list ap;
    char *p;

    va_start(ap, fmt);
    p = z_ta_vsnprintf(n, fmt, ap);
    va_end(ap);
    return p;
}

ZAPI char *z_ta_vsprintf(const char *fmt, va_list ap)
{
    va_list ap_copy;
    int needed;
    char *p;

    va_copy(ap_copy, ap);
    needed = vsnprintf(NULL, 0, fmt, ap_copy);
    va_end(ap_copy);
    if (needed < 0) return NULL;
    p = z_ta_alloc((size_t)needed + 1);
    if (!p) return NULL;
    vsnprintf(p, (size_t)needed + 1, fmt, ap);
    return p;
}

ZAPI char *z_ta_vsnprintf(size_t n, const char *fmt, va_list ap)
{
    char *p;

    p = z_ta_alloc(n);
    if (!p) return NULL;
    vsnprintf(p, n, fmt, ap);
    return p;
}

ZAPI z_string_view_t z_sv_from_cstr(const char *cstr)
{
    z_string_view_t sv;

    if (cstr) {
        sv.data  = cstr;
        sv.count = strlen(cstr);
    } else {
        sv.data  = NULL;
        sv.count = 0;
    }
    return sv;
}

ZAPI z_string_view_t z_sv_from_parts(const char *data, size_t count)
{
    z_string_view_t sv;

    sv.data  = data;
    sv.count = count;
    return sv;
}

ZAPI z_string_view_t z_sv_substr(z_string_view_t sv, size_t start, size_t count)
{
    z_string_view_t out;
    size_t remain;

    memset(&out, 0, sizeof(out));
    if (start >= sv.count) return out;
    remain = sv.count - start;
    if (count > remain) count = remain;
    out.data  = sv.data + start;
    out.count = count;
    return out;
}

static bool z__is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\v' || c == '\f';
}

ZAPI z_string_view_t z_sv_trim_left(z_string_view_t sv)
{
    size_t i;

    i = 0;
    while (i < sv.count && z__is_space(sv.data[i])) i++;
    sv.data  += i;
    sv.count -= i;
    return sv;
}

ZAPI z_string_view_t z_sv_trim_right(z_string_view_t sv)
{
    size_t n;

    n = sv.count;
    while (n > 0 && z__is_space(sv.data[n - 1])) n--;
    sv.count = n;
    return sv;
}

ZAPI z_string_view_t z_sv_trim(z_string_view_t sv)
{
    return z_sv_trim_right(z_sv_trim_left(sv));
}

ZAPI bool z_sv_starts_with(z_string_view_t sv, z_string_view_t prefix)
{
    if (prefix.count > sv.count) return false;
    if (prefix.count == 0) return true;
    return memcmp(sv.data, prefix.data, prefix.count) == 0;
}

ZAPI bool z_sv_ends_with(z_string_view_t sv, z_string_view_t suffix)
{
    if (suffix.count > sv.count) return false;
    if (suffix.count == 0) return true;
    return memcmp(sv.data + (sv.count - suffix.count), suffix.data, suffix.count) == 0;
}

ZAPI bool z_sv_equal(z_string_view_t a, z_string_view_t b)
{
    if (a.count != b.count) return false;
    if (a.count == 0) return true;
    return memcmp(a.data, b.data, a.count) == 0;
}

ZAPI bool z_sb_reserve(z_string_builder_t *sb, size_t need)
{
    size_t new_cap;
    char *p;

    if (need <= sb->capacity) return true;
    new_cap = sb->capacity ? sb->capacity : 32;
    while (new_cap < need) {
        if (new_cap > (size_t)-1 / 2) {
            new_cap = need;
            break;
        }
        new_cap *= 2;
    }
    p = (char *)realloc(sb->chars, new_cap);
    if (!p) return false;
    sb->chars    = p;
    sb->capacity = new_cap;
    return true;
}

ZAPI z_string_view_t z_sb_view(z_string_builder_t *sb)
{
    z_string_view_t sv;

    sv.data  = sb->chars;
    sv.count = sb->count;
    return sv;
}

ZAPI void z_sb_free(z_string_builder_t *sb)
{
    Z_FREE(sb->chars);
    sb->chars    = NULL;
    sb->count    = 0;
    sb->capacity = 0;
}

ZAPI void z_sb_clear(z_string_builder_t *sb)
{
    sb->count = 0;
}

ZAPI void z_sb_append_char(z_string_builder_t *sb, char c)
{
    if (!z_sb_reserve(sb, sb->count + 1)) return;
    sb->chars[sb->count++] = c;
}

ZAPI void z_sb_append_parts(z_string_builder_t *sb, const char *data, size_t n)
{
    if (n == 0) return;
    if (!z_sb_reserve(sb, sb->count + n)) return;
    memcpy(sb->chars + sb->count, data, n);
    sb->count += n;
}

ZAPI void z_sb_append_sv(z_string_builder_t *sb, z_string_view_t sv)
{
    z_sb_append_parts(sb, sv.data, sv.count);
}

ZAPI void z_sb_append_cstr(z_string_builder_t *sb, const char *cstr)
{
    if (!cstr) return;
    z_sb_append_parts(sb, cstr, strlen(cstr));
}

#endif /* Z_IMPLEMENTATION */

#ifndef Z_NOT_STRIP_OFF_PREFIX
    #define arrhdr z_arrhdr
    #define arrcnt z_arrcnt
    #define arrcap z_arrcap
    #define arrbegin z_arrbegin
    #define arrend z_arrend
    #define arr_push z_arr_push
    #define arr_pop z_arr_pop
    #define arr_foreach z_arr_foreach
    #define arr_clear z_arr_clear
    #define arr_free z_arr_free
    #define wrapinc z_wrapinc
    #define wrapdec z_wrapdec
    #define clamp z_clamp
    #define offset_of z_offset_of
    #define container_of z_container_of
    #define align_up z_align_up
    #define align_down z_align_down
    #define ta_alloc z_ta_alloc
    #define ta_save z_ta_save
    #define ta_restore z_ta_restore
    #define ta_strdup z_ta_strdup
    #define ta_strndup z_ta_strndup
    #define ta_sprintf z_ta_sprintf
    #define ta_snprintf z_ta_snprintf
    #define ta_vsprintf z_ta_vsprintf
    #define ta_vsnprintf z_ta_vsnprintf
    #define string_view_t z_string_view_t
    #define svfmt z_svfmt
    #define svarg z_svarg
    #define sv_from_cstr z_sv_from_cstr
    #define sv_from_parts z_sv_from_parts
    #define sv_substr z_sv_substr
    #define sv_trim_left z_sv_trim_left
    #define sv_trim_right z_sv_trim_right
    #define sv_trim z_sv_trim
    #define sv_starts_with z_sv_starts_with
    #define sv_ends_with z_sv_ends_with
    #define sv_equal z_sv_equal
    #define string_builder_t z_string_builder_t
    #define sb_view z_sb_view
    #define sb_free z_sb_free
    #define sb_clear z_sb_clear
    #define sb_reserve z_sb_reserve
    #define sb_append_char z_sb_append_char
    #define sb_append_sv z_sb_append_sv
    #define sb_append_cstr z_sb_append_cstr
    #define sb_append_parts z_sb_append_parts
    #define deque_header_t z_deque_header_t
    #define deqhdr z_deqhdr
    #define deqcnt z_deqcnt
    #define deqcap z_deqcap
    #define deqbegin z_deqbegin
    #define deqend z_deqend
    #define deqhead z_deqhead
    #define deqtail z_deqtail
    #define deq_grow z_deq_grow
    #define deq_push_back z_deq_push_back
    #define deq_push_front z_deq_push_front
    #define deq_pop_back z_deq_pop_back
    #define deq_pop_front z_deq_pop_front
    #define deq_clear z_deq_clear
    #define deq_free z_deq_free
    #define deq_foreach z_deq_foreach
#endif /* Z_NOT_STRIP_OFF_PREFIX */
