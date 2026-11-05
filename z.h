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
#include <stddef.h>


//////////////////////////////////////////////////////
// Dynamic Array
//////////////////////////////////////////////////////


#define Z_ARRAY_INITIALIZER NULL

#ifndef Z_ARRAY_INIT_CAPACITY
#define Z_ARRAY_INIT_CAPACITY 128
#endif

typedef struct {
    size_t count;
    size_t capacity;
} z_ArrayHeader;

#define z_arrhdr(a_)   ((z_ArrayHeader*)((char*)(a_)-sizeof(z_ArrayHeader)))
#define z_arrcnt(a_)   ((a_) ? z_arrhdr(a_)->count : 0)
#define z_arrcap(a_)   ((a_) ? z_arrhdr(a_)->capacity : 0)
#define z_arrbegin(a_) (a_)
#define z_arrend(a_)   ((a_) ? (a_) + z_arrcnt(a_) : NULL)

#define z_arr_grow(a_) do {                                                                          \
    z_ArrayHeader *h = (a_) ? z_arrhdr(a_) : NULL;                                                   \
    size_t old_cnt = z_arrcnt(a_);                                                                   \
    size_t old_cap = z_arrcap(a_);                                                                   \
    size_t new_cap = old_cap < Z_ARRAY_INIT_CAPACITY ? Z_ARRAY_INIT_CAPACITY : 2*old_cap;            \
    h = Z_REALLOC(h, sizeof(*h)+new_cap*sizeof(*(a_)));                                              \
    Z_ASSERT(h && "run out of memory");                                                              \
    h->count = old_cnt;                                                                              \
    h->capacity = new_cap;                                                                           \
    (a_) = (void*)(h + 1);                                                                           \
} while (0)
#define z_arr_push(a_, item_) do {         \
    if (z_arrcnt(a_) + 1 > z_arrcap(a_)) { \
        z_arr_grow(a_);                    \
    }                                      \
    (a_)[z_arrhdr(a_)->count++] = (item_); \
} while (0)
#define z_arr_pop(a_) (Z_ASSERT(z_arrcnt(a_) > 0 && "array underflow"), (a_)[--z_arrhdr(a_)->count])
#define z_arr_foreach(a_, T_, iter_) \
        for (T_ *iter_ = z_arrbegin(a_); iter_ != z_arrend(a_); iter_++)
#define z_arr_clear(a_) do { if (a_) z_arrhdr(a_)->count = 0; } while (0)
#define z_arr_free(a_) do { if (a_) Z_FREE(z_arrhdr(a_)); (a_) = NULL; } while (0)


//////////////////////////////////////////////////////
// Misc
//////////////////////////////////////////////////////


#define z_wrapinc(x_, bound_)               (((x_) + 1 >= (bound_)) ? 0 : (x_) + 1)
#define z_wrapdec(x_, bound_)               (((x_) == 0) ? ((bound_) - 1) : (x_) - 1)
#define z_clamp(x_, lo_, hi_)               ((x_) < (lo_) ? (lo_) : ((x_) > (hi_) ? (hi_) : (x_)))
#define z_offset_of(T, member_)             ((size_t)&(((T*)0)->member_))
#define z_container_of(ptr_, T, member_)    ((T*)((char*)(ptr_) - z_offset_of(T, member_)))
#define z_align_up(x_, align_)              (((x_) + (align_) - 1) & ~((align_) - 1))
#define z_align_down(x_, align_)            ((x_) & ~((align_) - 1))


//////////////////////////////////////////////////////
// Temp Allocator
//////////////////////////////////////////////////////


#ifndef Z_TEMP_BUFFER_CAPACITY
#define Z_TEMP_BUFFER_CAPACITY (2 << 10)
#endif

ZAPI void  *z_ta_alloc(size_t n);
ZAPI size_t z_ta_save(void);
ZAPI void   z_ta_restore(size_t checkpoint);
ZAPI char  *z_ta_strdup(const char *s);
ZAPI char  *z_ta_strndup(const char *s, size_t n);
ZAPI char  *z_ta_sprintf(const char *format, ...);
ZAPI char  *z_ta_snprintf(size_t n, const char *format, ...);
ZAPI char  *z_ta_vsprintf(const char *format, va_list args);
ZAPI char  *z_ta_vsnprintf(size_t n, const char *format, va_list args);


//////////////////////////////////////////////////////
// String View
//////////////////////////////////////////////////////


typedef struct {
    size_t count;
    const char *data;
} z_StringView;

#define z_svfmt "%.*s"
#define z_svarg(sv) (int)(sv).count, (sv).data

ZAPI z_StringView z_sv_from_cstr(const char *cstr);
ZAPI z_StringView z_sv_from_parts(const char *data, size_t count);
ZAPI z_StringView z_sv_substr(z_StringView sv, size_t start, size_t count);
ZAPI z_StringView z_sv_trim_left(z_StringView sv);
ZAPI z_StringView z_sv_trim_right(z_StringView sv);
ZAPI z_StringView z_sv_trim(z_StringView sv);
ZAPI bool        z_sv_starts_with(z_StringView sv, z_StringView prefix);
ZAPI bool        z_sv_ends_with(z_StringView sv, z_StringView suffix);
ZAPI bool        z_sv_equal(z_StringView a, z_StringView b);


//////////////////////////////////////////////////////
// String Builder
//////////////////////////////////////////////////////


typedef struct {
    size_t count;
    size_t capacity;
    char *chars;
} z_StringBuilder;

ZAPI z_StringView z_sb_view(z_StringBuilder *sb);
ZAPI void         z_sb_free(z_StringBuilder *sb);
ZAPI void         z_sb_clear(z_StringBuilder *sb);
ZAPI bool         z_sb_reserve(z_StringBuilder *sb, size_t need);
ZAPI void         z_sb_append_char(z_StringBuilder *sb, char c);
ZAPI void         z_sb_append_sv(z_StringBuilder *sb, z_StringView sv);
ZAPI void         z_sb_append_cstr(z_StringBuilder *sb, const char *cstr);
ZAPI void         z_sb_append_parts(z_StringBuilder *sb, const char *data, size_t n);


//////////////////////////////////////////////////////
// Dynamic Deque
//////////////////////////////////////////////////////


typedef struct {
    size_t count;
    size_t capacity;
    size_t head;
} z_DequeHeader;

#ifndef Z_DEQUE_INIT_CAPACITY
#define Z_DEQUE_INIT_CAPACITY 128
#endif

#define Z_DEQUE_INITIALIZER NULL

#define z_deqhdr(d_)   ((z_DequeHeader*)((char*)(d_)-sizeof(z_DequeHeader)))
#define z_deqcnt(d_)   ((d_) ? z_deqhdr(d_)->count : 0)
#define z_deqcap(d_)   ((d_) ? z_deqhdr(d_)->capacity : 0)
#define z_deqbegin(d_) ((d_) ? (d_) + z_deqhdr(d_)->head : NULL)
#define z_deqend(d_)   ((d_) ? (d_) + (z_deqhdr(d_)->head + z_deqcnt(d_)) % z_deqcap(d_) : NULL)
#define z_deqhead(d_)  (Z_ASSERT(z_deqcnt(d_) > 0 && "deque is empty"), (d_)[z_deqhdr(d_)->head])
#define z_deqtail(d_)  (Z_ASSERT(z_deqcnt(d_) > 0 && "deque is empty"), \
        (d_)[(z_deqhdr(d_)->head + z_deqcnt(d_) - 1) % z_deqcap(d_)])

#define z_deq_grow(d_) do {                                                                           \
    z_DequeHeader *h = (d_) ? z_deqhdr(d_) : NULL;                                                    \
    size_t old_cnt  = z_deqcnt(d_);                                                                   \
    size_t old_cap  = z_deqcap(d_);                                                                   \
    size_t old_head = h ? h->head : 0;                                                                \
    size_t new_cap  = old_cap < Z_DEQUE_INIT_CAPACITY ? Z_DEQUE_INIT_CAPACITY : 2*old_cap;            \
    h = Z_MALLOC(sizeof(*h)+new_cap*sizeof(*(d_)));                                                   \
    Z_ASSERT(h && "run out of memory");                                                               \
    memset(h, 0, sizeof(*h)+new_cap*sizeof(*(d_)));                                                   \
    if (d_) {                                                                                         \
        char *p = (char*)(h + 1);                                                                     \
        memmove(p, (d_)+old_head, (old_cap-old_head)*sizeof(*(d_)));                                  \
        size_t offset = (old_cap-old_head)*sizeof(*(d_));                                             \
        memmove(p+offset, (d_), old_head*sizeof(*(d_)));                                              \
    }                                                                                                 \
    h->count    = old_cnt;                                                                            \
    h->capacity = new_cap;                                                                            \
    h->head     = 0;                                                                                  \
    z_deq_free(d_);                                                                                   \
    (d_) = (void*)(h + 1);                                                                            \
} while (0)
#define z_deq_push_back(d_, item_) do {                 \
    if (z_deqcnt(d_) + 1 > z_deqcap(d_)) {              \
        z_deq_grow(d_);                                 \
    }                                                   \
    z_DequeHeader *h = z_deqhdr(d_);                    \
    (d_)[(h->head + h->count) % h->capacity] = (item_); \
    h->count++;                                         \
} while (0)
#define z_deq_push_front(d_, item_) do {                 \
    if (z_deqcnt(d_) + 1 > z_deqcap(d_)) {               \
        z_deq_grow(d_);                                  \
    }                                                    \
    z_DequeHeader *h = z_deqhdr(d_);                     \
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
#define z_deq_foreach(d_, T_, iter_)                                             \
        for (T_ *iter_ = z_deqbegin(d_), *z_deq_end_ = z_deqend(d_),            \
                 *z_deq_first_ = iter_;                                          \
             z_deqcnt(d_) > 0 && (iter_ != z_deq_end_ || z_deq_first_);          \
             z_deq_first_ = NULL,                                                \
             iter_ = (iter_ + 1) == (d_) + z_deqcap(d_) ? (d_) : (iter_ + 1))


//////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////


typedef enum {
    ZLOG_TRACE = 1,
    ZLOG_DEBUG,
    ZLOG_INFO,
    ZLOG_WARN,
    ZLOG_ERROR,
    ZLOG_FATAL,
} z_LogLevel;

typedef struct {
    z_LogLevel level;
    FILE *sink;
    const char *filepath;
    bool append;
    bool hold; // auto cleanup sink?
} z_LogOpt;

#define zlog_init(...) zlog_init_with_opt((z_LogOpt){__VA_ARGS__})
#define zlog(level_, format_, ...) do { \
    zlog_write(level_, __FILE__, __LINE__, format_, ##__VA_ARGS__); \
} while (0)

ZAPI void zlog_init_with_opt(z_LogOpt opt);
ZAPI void zlog_fini(void);
ZAPI void zlog_write(z_LogLevel level, const char *file, int line, const char *format, ...) __attribute__((format(printf, 4, 5)));


//////////////////////////////////////////////////////
// Compatibility Layer
//////////////////////////////////////////////////////


#ifdef _WIN32

#include <windows.h>
#include <errno.h>

#define DT_UNKNOWN 0
#define DT_DIR     1
#define DT_REG     2

struct dirent {
    char d_name[MAX_PATH + 1];
    int d_type;
};

typedef struct {
    HANDLE          hFind;
    WIN32_FIND_DATA data;
    struct dirent   ent;
    bool            first; // first call readdir?
} DIR;

DIR *opendir(const char *path);
struct dirent *readdir(DIR *dp);
int closedir(DIR *dp);

#else

#include <dirent.h>

#endif // _WIN32

#endif // Z_H

#ifdef Z_IMPLEMENTATION

static z_LogOpt z__logger = {
    .level    = ZLOG_INFO,
    .sink     = NULL,
    .filepath = NULL,
    .append   = false,
    .hold     = false,
};

ZAPI void zlog_init_with_opt(z_LogOpt opt)
{
    if (opt.level > 0) z__logger.level = opt.level;
    if (opt.sink) {
        if (z__logger.hold) fclose(z__logger.sink);
        z__logger.sink = opt.sink;
        z__logger.hold = false;
    }
    if (opt.filepath) {
        const char *mode = opt.append ? "a" : "w";
        if (z__logger.hold) fclose(z__logger.sink);
        FILE *fp = fopen(opt.filepath, mode);
        z__logger.sink   = fp ? fp : stderr;
        z__logger.append = opt.append;
        z__logger.hold   = fp != NULL;
    }
    if (!z__logger.sink) z__logger.sink = stderr;
}

ZAPI void zlog_fini(void)
{
    if (z__logger.hold && z__logger.sink) fclose(z__logger.sink);
    z__logger.sink     = NULL;
    z__logger.filepath = NULL;
    z__logger.append   = false;
    z__logger.hold     = false;
}

ZAPI void zlog_write(z_LogLevel level, const char *file, int line, const char *format, ...)
{
    if (level < z__logger.level) return;

    const char *level_name = NULL;
    switch (level) {
    case ZLOG_TRACE: level_name = "TRACE"; break;
    case ZLOG_DEBUG: level_name = "DEBUG"; break;
    case ZLOG_INFO:  level_name = "INFO";  break;
    case ZLOG_WARN:  level_name = "WARN";  break;
    case ZLOG_ERROR: level_name = "ERROR"; break;
    case ZLOG_FATAL: level_name = "FATAL"; break;
    default:         Z_ASSERT(0 && "invalid level");
    }

    va_list args;
    va_start(args, format);
    char buffer[1024] = {0};
    Z_ASSERT(vsnprintf(buffer, sizeof(buffer), format, args) >= 0 && "vsnprintf format error");
    va_end(args);

    FILE *fp = z__logger.sink ? z__logger.sink : stderr;
    fprintf(fp, "%s:%d: [%s] %s\n", file, line, level_name, buffer);
}

#ifdef _WIN32

DIR *opendir(const char *dirpath)
{
    if (!dirpath) {
        errno = EINVAL;
        return NULL;
    }

    char buffer[MAX_PATH] = {0};
    size_t len = strlen(dirpath);
    if (len == 0) {
        snprintf(buffer, MAX_PATH, ".\\*");
    } else if (len > 0 && (dirpath[len-1] == '\\' || dirpath[len-1] == '/')) {
        snprintf(buffer, MAX_PATH, "%s*", dirpath);
    } else {
        snprintf(buffer, MAX_PATH, "%s\\*", dirpath);
    }

    DIR *dp = malloc(sizeof(DIR));
    if (!dp) {
        errno = ENOMEM;
        return NULL;
    }
    memset(dp, 0, sizeof(DIR));

    dp->hFind = FindFirstFile(buffer, &dp->data);
    if (dp->hFind == INVALID_HANDLE_VALUE) {
        free(dp);
        errno = ENOENT;
        return NULL;
    }
    dp->first = true;

    return dp;
}

struct dirent *readdir(DIR *dp)
{
    if (!dp) {
        errno = EINVAL;
        return NULL;
    }

    if (dp->first) {
        dp->first = false;
    } else {
        if (!FindNextFile(dp->hFind, &dp->data)) {
            if (GetLastError() == ERROR_NO_MORE_FILES) return NULL;
            errno = EIO;
            return NULL;
        }
    }

    strncpy(dp->ent.d_name, dp->data.cFileName, sizeof(dp->ent.d_name)-1);
    dp->ent.d_name[sizeof(dp->ent.d_name)-1] = '\0';
    if (dp->data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        dp->ent.d_type = DT_DIR;
    } else {
        dp->ent.d_type = DT_REG;
    }

    return &dp->ent;
}

int closedir(DIR *dp)
{
    if (!dp) {
        errno = EINVAL;
        return -1;
    }
    FindClose(dp->hFind);
    free(dp);
    return 0;
}

#endif // _WIN32

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
typedef max_align_t z_TempAlignment;
#define Z_TEMP_ALIGNMENT _Alignof(max_align_t)
#else
typedef union {
    long double              long_double;
    long long                long_long;
    void                    *pointer;
    void                   (*function)(void);
} z_TempAlignment;
#define Z_TEMP_ALIGNMENT sizeof(z_TempAlignment)
#endif

typedef union {
    z_TempAlignment alignment;
    unsigned char   bytes[Z_TEMP_BUFFER_CAPACITY];
} z_TempBuffer;

static z_TempBuffer z__temp_buffer = {0};
static size_t z__temp_used = 0;

ZAPI void *z_ta_alloc(size_t n)
{
    if (z__temp_used > Z_TEMP_BUFFER_CAPACITY) return NULL;

    size_t padding = z__temp_used % Z_TEMP_ALIGNMENT;
    if (padding) padding = Z_TEMP_ALIGNMENT - padding;

    size_t available = Z_TEMP_BUFFER_CAPACITY - z__temp_used;
    if (padding > available || n > available - padding) return NULL;

    void *p = &z__temp_buffer.bytes[z__temp_used + padding];
    z__temp_used += padding + n;
    return p;
}

ZAPI size_t z_ta_save(void)
{
    return z__temp_used;
}

ZAPI void z_ta_restore(size_t checkpoint)
{
    z__temp_used = checkpoint;
}

ZAPI char *z_ta_strdup(const char *s)
{
    size_t len = strlen(s);
    char *p = z_ta_alloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

ZAPI char *z_ta_strndup(const char *s, size_t n)
{
    size_t len = strnlen(s, n);
    char *p = z_ta_alloc(len + 1);
    if (p) {
        memcpy(p, s, len);
        p[len] = '\0';
    }
    return p;
}

ZAPI char *z_ta_sprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char *p = z_ta_vsprintf(format, args);
    va_end(args);
    return p;
}

ZAPI char *z_ta_snprintf(size_t n, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char *p = z_ta_vsnprintf(n, format, args);
    va_end(args);
    return p;
}

ZAPI char *z_ta_vsprintf(const char *format, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    if (needed < 0) return NULL;
    char *p = z_ta_alloc((size_t)needed+1);
    if (!p) return NULL;
    vsnprintf(p, (size_t)needed+1, format, args);
    return p;
}

ZAPI char *z_ta_vsnprintf(size_t n, const char *format, va_list args)
{
    char *p = z_ta_alloc(n);
    if (!p) return NULL;
    vsnprintf(p, n, format, args);
    return p;
}

ZAPI z_StringView z_sv_from_cstr(const char *cstr)
{
    z_StringView sv = {0};
    if (cstr) {
        sv.data  = cstr;
        sv.count = strlen(cstr);
    } else {
        sv.data  = NULL;
        sv.count = 0;
    }
    return sv;
}

ZAPI z_StringView z_sv_from_parts(const char *data, size_t count)
{
    return (z_StringView) {
        .data  = data,
        .count = count
    };
}

ZAPI z_StringView z_sv_substr(z_StringView sv, size_t start, size_t count)
{
    z_StringView out = {0};
    if (start >= sv.count) return out;
    size_t remain = sv.count - start;
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

ZAPI z_StringView z_sv_trim_left(z_StringView sv)
{
    size_t i = 0;
    while (i < sv.count && z__is_space(sv.data[i])) i++;
    sv.data  += i;
    sv.count -= i;
    return sv;
}

ZAPI z_StringView z_sv_trim_right(z_StringView sv)
{
    size_t n = sv.count;
    while (n > 0 && z__is_space(sv.data[n - 1])) n--;
    sv.count = n;
    return sv;
}

ZAPI z_StringView z_sv_trim(z_StringView sv)
{
    return z_sv_trim_right(z_sv_trim_left(sv));
}

ZAPI bool z_sv_starts_with(z_StringView sv, z_StringView prefix)
{
    if (prefix.count > sv.count) return false;
    if (prefix.count == 0) return true;
    return memcmp(sv.data, prefix.data, prefix.count) == 0;
}

ZAPI bool z_sv_ends_with(z_StringView sv, z_StringView suffix)
{
    if (suffix.count > sv.count) return false;
    if (suffix.count == 0) return true;
    return memcmp(sv.data + (sv.count - suffix.count), suffix.data, suffix.count) == 0;
}

ZAPI bool z_sv_equal(z_StringView a, z_StringView b)
{
    if (a.count != b.count) return false;
    if (a.count == 0) return true;
    return memcmp(a.data, b.data, a.count) == 0;
}

ZAPI bool z_sb_reserve(z_StringBuilder *sb, size_t need)
{
    if (need <= sb->capacity) return true;
    size_t new_cap = sb->capacity ? sb->capacity : 32;
    while (new_cap < need) {
        if (new_cap > (size_t)-1 / 2) {
            new_cap = need;
            break;
        }
        new_cap *= 2;
    }
    char *p = (char *)Z_REALLOC(sb->chars, new_cap);
    if (!p) return false;
    sb->chars    = p;
    sb->capacity = new_cap;
    return true;
}

ZAPI z_StringView z_sb_view(z_StringBuilder *sb)
{
    return (z_StringView) {
        .data  = sb->chars,
        .count = sb->count
    };
}

ZAPI void z_sb_free(z_StringBuilder *sb)
{
    Z_FREE(sb->chars);
    sb->chars    = NULL;
    sb->count    = 0;
    sb->capacity = 0;
}

ZAPI void z_sb_clear(z_StringBuilder *sb)
{
    sb->count = 0;
}

ZAPI void z_sb_append_char(z_StringBuilder *sb, char c)
{
    if (!z_sb_reserve(sb, sb->count + 1)) return;
    sb->chars[sb->count++] = c;
}

ZAPI void z_sb_append_parts(z_StringBuilder *sb, const char *data, size_t n)
{
    if (n == 0) return;
    if (!z_sb_reserve(sb, sb->count + n)) return;
    memcpy(sb->chars + sb->count, data, n);
    sb->count += n;
}

ZAPI void z_sb_append_sv(z_StringBuilder *sb, z_StringView sv)
{
    z_sb_append_parts(sb, sv.data, sv.count);
}

ZAPI void z_sb_append_cstr(z_StringBuilder *sb, const char *cstr)
{
    if (!cstr) return;
    z_sb_append_parts(sb, cstr, strlen(cstr));
}

#endif // Z_IMPLEMENTATION

#ifndef Z_NOT_STRIP_OFF_PREFIX
    #define arrhdr           z_arrhdr
    #define arrcnt           z_arrcnt
    #define arrcap           z_arrcap
    #define arrbegin         z_arrbegin
    #define arrend           z_arrend
    #define arr_push         z_arr_push
    #define arr_pop          z_arr_pop
    #define arr_foreach      z_arr_foreach
    #define arr_clear        z_arr_clear
    #define arr_free         z_arr_free
    #define wrapinc          z_wrapinc
    #define wrapdec          z_wrapdec
    #define clamp            z_clamp
    #define offset_of        z_offset_of
    #define container_of     z_container_of
    #define align_up         z_align_up
    #define align_down       z_align_down
    #define ta_alloc         z_ta_alloc
    #define ta_save          z_ta_save
    #define ta_restore       z_ta_restore
    #define ta_strdup        z_ta_strdup
    #define ta_strndup       z_ta_strndup
    #define ta_sprintf       z_ta_sprintf
    #define ta_snprintf      z_ta_snprintf
    #define ta_vsprintf      z_ta_vsprintf
    #define ta_vsnprintf     z_ta_vsnprintf
    #define string_view_t    z_StringView
    #define svfmt            z_svfmt
    #define svarg            z_svarg
    #define sv_from_cstr     z_sv_from_cstr
    #define sv_from_parts    z_sv_from_parts
    #define sv_substr        z_sv_substr
    #define sv_trim_left     z_sv_trim_left
    #define sv_trim_right    z_sv_trim_right
    #define sv_trim          z_sv_trim
    #define sv_starts_with   z_sv_starts_with
    #define sv_ends_with     z_sv_ends_with
    #define sv_equal         z_sv_equal
    #define string_builder_t z_StringBuilder
    #define sb_view          z_sb_view
    #define sb_free          z_sb_free
    #define sb_clear         z_sb_clear
    #define sb_reserve       z_sb_reserve
    #define sb_append_char   z_sb_append_char
    #define sb_append_sv     z_sb_append_sv
    #define sb_append_cstr   z_sb_append_cstr
    #define sb_append_parts  z_sb_append_parts
    #define deque_header_t   z_DequeHeader
    #define deqhdr           z_deqhdr
    #define deqcnt           z_deqcnt
    #define deqcap           z_deqcap
    #define deqbegin         z_deqbegin
    #define deqend           z_deqend
    #define deqhead          z_deqhead
    #define deqtail          z_deqtail
    #define deq_grow         z_deq_grow
    #define deq_push_back    z_deq_push_back
    #define deq_push_front   z_deq_push_front
    #define deq_pop_back     z_deq_pop_back
    #define deq_pop_front    z_deq_pop_front
    #define deq_clear        z_deq_clear
    #define deq_free         z_deq_free
    #define deq_foreach      z_deq_foreach
#endif // Z_NOT_STRIP_OFF_PREFIX

