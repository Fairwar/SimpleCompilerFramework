#ifndef SCF_STRING_H
#define SCF_STRING_H

#include <stddef.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>

typedef struct SCF_STRING
{
    int capacity; //容量
    size_t len;   //长度
    char *data;   //字符串头指针
} scf_string_t;

scf_string_t *scf_string_alloc();
void scf_string_free(scf_string_t *s);

scf_string_t *scf_string_clone(scf_string_t *s);
scf_string_t *scf_string_cstr(const char *str);
scf_string_t *scf_string_cstr_len(const char *str, size_t len);

int scf_string_cmp(const scf_string_t *s0, const scf_string_t *s1);
int scf_string_cmp_cstr(const scf_string_t *s, const char *str);
int scf_string_cmp_cstr_len(const scf_string_t *s, const char *str, int len);

int scf_string_copy(scf_string_t *dst, const scf_string_t *src);
int scf_string_cat(scf_string_t *dst, const scf_string_t *src);
int scf_string_cat_cstr(scf_string_t *dst, const char *src);
int scf_string_cat_cstr_len(scf_string_t *dst, const char *src, int len);

#endif
