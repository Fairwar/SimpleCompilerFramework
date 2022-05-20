#include "scf_string.h"

#define SCF_STRING_CAPACITY_INC 4

scf_string_t *scf_string_alloc()
{
    scf_string_t *s = malloc(sizeof(scf_string_t));
    if (!s)
    {
        return NULL;
    }

    s->data = malloc(SCF_STRING_CAPACITY_INC + 1);
    if (!s)
    {
        free(s);
        return NULL;
    }

    s->capacity = SCF_STRING_CAPACITY_INC;
    s->len = 0;
    s->data[0] = '\0';
    return s;
}
void scf_string_free(scf_string_t *s)
{
    if (!s)
    {
        return;
    }

    if (s->capacity > 0)
    {
        free(s->data);
    }

    free(s);
    s = NULL;
}

scf_string_t *scf_string_clone(scf_string_t *s)
{

    //检查输入指针
    if (!s)
    {
        return NULL;
    }

    //分配空间
    scf_string_t *s_ret = malloc(sizeof(scf_string_t));
    if (!s_ret)
    {
        return NULL;
    }

    //拷贝capacity
    if (s->capacity > 0)
    {
        s_ret->capacity = s->capacity;
    }
    else if (s->len > 0)
    {
        s_ret->capacity = s->len;
    }
    else
    {
        s_ret->capacity = SCF_STRING_CAPACITY_INC;
    }

    //分配data空间
    s_ret->data = malloc(SCF_STRING_CAPACITY_INC + 1);
    if (!s_ret->data)
    {
        return NULL;
    }

    //拷贝len,data
    s_ret->len = s->len;
    if (s->len > 0)
    {
        memcpy(s_ret->data, s->data, s->len);
    }
    s_ret->data[s_ret->len] = '\0';

    return s_ret;
}
scf_string_t *scf_string_cstr(const char *str)
{
    if (!str)
    {
        return NULL;
    }
    return scf_string_cstr_len(str, strlen(str));
}
scf_string_t *scf_string_cstr_len(const char *str, size_t len)
{
    if (!str)
    {
        return NULL;
    }

    scf_string_t s;
    s.capacity = -1;
    s.len = len;
    s.data = (char *)str;
    return scf_string_clone(&s);
}

int scf_string_cmp(const scf_string_t *s0, const scf_string_t *s1)
{
    if (!s0 || !s1 || !s0->data || !s1->data)
    {
        return ERROR_BAD_ARGUMENTS;
    }

    if (s0->len < s1->len)
    {
        return -1;
    }
    else if (s0->len > s1->len)
    {
        return 1;
    }
    else
        return strcmp(s0, s1);
}
int scf_string_cmp_cstr(const scf_string_t *s, const char *str)
{
    if (!s || !s->data || !str)
    {
        return ERROR_BAD_ARGUMENTS;
    }

    return scf_string_cmp_cstr_len(s, str, strlen(str));
}
int scf_string_cmp_cstr_len(const scf_string_t *s, const char *str, int len)
{
    if (!s || !s->data || !strchr)
    {
        return ERROR_BAD_ARGUMENTS;
    }

    scf_string_t s1;
    s1.capacity = -1;
    s1.len = len;
    s1.data = (char *)str;

    scf_string_cmp(s, &s1);
}

int scf_string_copy(scf_string_t *dst, const scf_string_t *src)
{
    if (!dst || !src || !dst->data || !src->data)
    {
        return ERROR_BAD_ARGUMENTS;
    }

    if (dst->capacity < src->len)
    {
        char *p = realloc(dst->data, src->len + 1);
        if (!p)
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        dst->data = p;
        dst->capacity = src->len;
    }
    memcpy(dst->data, src->data, src->len);
    dst->data[src->len] = '\0';
    dst->len = src->len;
}
int scf_string_cat(scf_string_t *dst, const scf_string_t *src)
{
    if (!dst || !src || !dst->data || !src->data)
    {
        return ERROR_BAD_ARGUMENTS;
    }

    if (dst->len + src->len > dst->capacity)
    {
        char *p = realloc(dst->data, dst->len + src->len + SCF_STRING_CAPACITY_INC + 1);
        if (!p)
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        dst->data = p;
        dst->capacity = dst->len + src->len + SCF_STRING_CAPACITY_INC;
    }
    memcpy(dst->data+dst->len, src->data, src->len);
    dst->len += src->len;
    dst->data[dst->len] = '\0';
    return 0;
}
int scf_string_cat_cstr(scf_string_t *dst, const char *src)
{
    if (!dst || !dst->data || !src)
    {
        return ERROR_BAD_ARGUMENTS;
    }

    return scf_string_cat_cstr_len(dst, src, strlen(src));
}
int scf_string_cat_cstr_len(scf_string_t *dst, const char *src, int len)
{
    if (!dst || !dst->len || !src)
    {
        return ERROR_BAD_ARGUMENTS;
    }
    scf_string_t temp;
    temp.capacity = -1;
    temp.len = len;
    temp.data = (char *)src;

    return scf_string_cat(dst, &temp);
}