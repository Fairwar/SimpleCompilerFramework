#ifndef SCF_LEX_H
#define SCF_LEX_H

#include "scf_lex_word.h"

typedef struct
{
    char*              text;
    enum scf_lex_words type;
}scf_lex_key_word_t;

typedef struct
{
    char    origin;
    char    escape;
}scf_lex_escape_char_t;

typedef struct
{
    scf_list_t      list;
    scf_string_t*   message;
    scf_string_t*   file;
    int             line;
    int             pos;
}scf_lex_error_t;

typedef struct
{
    scf_list_t  list;
    char        c;
}scf_lex_char_t;

typedef struct
{
    scf_list_t  word_list_head;
    scf_list_t  error_list_head;

    scf_list_t  char_list_head;

    FILE*       fp;

    int         num_id;
    
    scf_string_t* file;
    int         read_lines;
    int         read_pos;
}scf_lex_t;

scf_lex_error_t*    scf_lex_error_alloc(scf_string_t*file, int line, int pos);
void                scf_lex_error_free(scf_lex_error_t* e);

int scf_lex_open(scf_lex_t** plex, const char* path);
int scf_lex_close(scf_lex_t* lex);

int scf_lex_push_word(scf_lex_t* lex, scf_lex_word_t* word);
int scf_lex_pop_word(scf_lex_t* lex, scf_lex_word_t** pword);


#endif