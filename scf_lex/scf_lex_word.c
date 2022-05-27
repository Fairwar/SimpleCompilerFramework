#include "scf_lex_word.h"

scf_lex_word_t* scf_lex_word_alloc(scf_string_t* file, int line, int pos, enum scf_lex_words type)
{
    assert(file);

    scf_lex_word_t* w = calloc(1,sizeof(scf_lex_word_t));
    assert(w);
    
    w->file = scf_string_clone(file);
    assert(w->file);

    w->type = type;
    w->line = line;
    w->pos  = pos;

    return w;
}

scf_lex_word_t* scf_lex_word_clone(scf_lex_word_t* w)
{
    scf_lex_word_t* w1=calloc(1,sizeof(scf_lex_word_t));
    assert(w1);

    scf_list_init(&w1->list);
    w1->type = w->type;

    switch (w->type)
    {
        case SCF_LEX_WORD_CONST_CHAR:
            w1->data.c = w->data.c;
            break;
        case SCF_LEX_WORD_CONST_STRING:
            w1->data.s = scf_string_clone(w->data.s);
            break;
        case SCF_LEX_WORD_CONST_INT:
            w1->data.i = w->data.i;
            break;
        case SCF_LEX_WORD_CONST_FLOAT:
            w1->data.f = w->data.f;
            break;
        case SCF_LEX_WORD_CONST_BOOL:
            w1->data.b = w->data.b;
            break;       
        default:
            break;
    }

    w1->text = scf_string_clone(w->text);
    w1->file = scf_string_clone(w->file);
    w1->line = w->line;
    w1->pos  = w->pos;
    
    return w1;
}

void scf_lex_word_free(scf_lex_word_t* w)
{
    assert(w);

    if(w->type == SCF_LEX_WORD_CONST_STRING)
    {
        scf_string_free(w->data.s);
    }

    if(w->text)
    {
        scf_string_free(w->text);
    }

    if(w->file)
    {
        scf_string_free(w->file);
    }

    free(w);
    w=NULL;
}

int scf_lex_word_set_data(scf_lex_word_t* w, scf_string_t* text, ...){
        va_list ap;
        va_start(ap, text);
        int base = va_arg(ap, int);
        va_end(ap);
    if(w->type==SCF_LEX_WORD_CONST_INT){
        return scf_string_to_int(text,&(w->data.i),base);
    }else if(w->type==SCF_LEX_WORD_CONST_FLOAT){
        return scf_string_to_float(text,&(w->data.f));
    }
}