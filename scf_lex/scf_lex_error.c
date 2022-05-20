#include"scf_lex_error.h"

scf_lex_error_t*    scf_lex_error_alloc(scf_string_t* file, int line, int pos)
{
    assert(file);

    scf_lex_error_t* e = calloc(1,sizeof(scf_lex_error_t));
    assert(e);
    
    e->file = scf_string_clone(file);
    assert(e->file);

    e->line = line;
    e->pos  = pos;

    return e;
}
void scf_lex_error_free(scf_lex_error_t* e)
{
    assert(e);

    if(e->file)
    {
        scf_string_free(e->file);
    }

    if(e->message)
    {
        scf_string_free(e->message);
    }

    free(e);
    e=NULL;
}