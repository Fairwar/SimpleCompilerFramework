#ifndef SCF_LEX_WORD_H
#define SCF_LEX_WORD_H

#include "scf_string.h"
#include "scf_list.h"

enum scf_lex_words{
    SCF_LEX_WORD_EOF=0,     // EOF
    
    SCF_LEX_WORD_PLUS,      // +
    SCF_LEX_WORD_MINUS,     // -
    SCF_LEX_WORD_STAR,      // *
    SCF_LEX_WORD_DIV,       // /
    SCF_LEX_WORD_MOD,       // %

    SCF_LEX_WORD_INC,       // ++
    SCF_LEX_WORD_DEC,       // --

    SCF_LEX_WORD_SHL,           // <<
    SCF_LEX_WORD_SHR,           // >>

    SCF_LEX_WORD_AND,           // &
    SCF_LEX_WORD_OR,            // |
    SCF_LEX_WORD_NOT,           // ~
    SCF_LEX_WORD_XOR,           // ^

    SCF_LEX_WORD_LOGIC_AND,     // &&
    SCF_LEX_WORD_LOGIC_OR,      // ||
    SCF_LEX_WORD_LOGIC_NOT,     // !

    SCF_LEX_WORD_ASSIGN,        // =
    SCF_LEX_WORD_ADD_ASSIGN,    // +=
    SCF_LEX_WORD_SUB_ASSIGN,    // -=
    SCF_LEX_WORD_MUL_ASSIGN,    // *=
    SCF_LEX_WORD_DIV_ASSIGN,    // /=
    SCF_LEX_WORD_MOD_ASSIGN,    // %=
    SCF_LEX_WORD_SHL_ASSIGN,    // <<=
    SCF_LEX_WORD_SHR_ASSIGN,    // >>=
    SCF_LEX_WORD_AND_ASSIGN,    // &=
    SCF_LEX_WORD_OR_ASSIGN,     // |=
    SCF_LEX_WORD_XOR_ASSIGN,    // ^=

    SCF_LEX_WORD_LT,            // <
    SCF_LEX_WORD_RT,            // >

    SCF_LEX_WORD_EQ,            // ==
    SCF_LEX_WORD_NE,            // !=
    SCF_LEX_WORD_LE,            // <=
    SCF_LEX_WORD_GE,            // >=

    SCF_LEX_WORD_LS,            // [
    SCF_LEX_WORD_RS,            // ]

    SCF_LEX_WORD_LP,            // (
    SCF_LEX_WORD_RP,            // )

    SCF_LEX_WORD_ARROW,         // ->
    SCF_LEX_WORD_DOT,           // .
    SCF_LEX_WORD_TO,            // ..
    SCF_LEX_WORD_VAR_ARGS,      // ...

    SCF_LEX_WORD_LB,            // {
    SCF_LEX_WORD_RB,            // }

    SCF_LEX_WORD_COMMA,         // ,
    SCF_LEX_WORD_SEMICOLON,     // ;
    SCF_LEX_WORD_COLON,         // :
    SCF_LEX_WORD_SPACE,         // ' '

    // for branching
    SCF_LEX_WORD_KEY_IF,        // if
    SCF_LEX_WORD_KEY_ELSE,      // else
    SCF_LEX_WORD_KEY_SWITCH,    // switch
    SCF_LEX_WORD_KEY_CASE,      // case
    SCF_LEX_WORD_KEY_DEFAULT,   // default

    // for recycling
    SCF_LEX_WORD_KEY_FOR,       // for
    SCF_LEX_WORD_KEY_IN,        // in
    SCF_LEX_WORD_KEY_WHILE,     // while
    SCF_LEX_WORD_KEY_BREAK,     // break
    SCF_LEX_WORD_KEY_CONTINUE,  // continue

    SCF_LEX_WORD_KEY_RETURN,    // return
    SCF_LEX_WORD_KEY_GOTO,      // goto
    
    // for error 
    SCF_LEX_WORD_KEY_TRY,       // try
    SCF_LEX_WORD_KEY_THROW,     // throw
    SCF_LEX_WORD_KEY_CATCH,     // catch
    SCF_LEX_WORD_KEY_ERROR,     // error
    
    SCF_LEX_WORD_KEY_SIZEOF,    // sizeof

    // basic type
    SCF_LEX_WORD_KEY_VOID,      // void
    SCF_LEX_WORD_KEY_CHAR,      // char
    SCF_LEX_WORD_KEY_STRING,    // string
    SCF_LEX_WORD_KEY_INT,       // int
    SCF_LEX_WORD_KEY_FLOAT,     // float
    SCF_LEX_WORD_KEY_BOOL,      // bool

    // for new type
    SCF_LEX_WORD_KEY_CLASS,     // class
    SCF_LEX_WORD_KEY_UNION,     // union
    SCF_LEX_WORD_KEY_STRUCT,    // struct

    // autority
    SCF_LEX_WORD_KEY_PRIVATE,   // private
    SCF_LEX_WORD_KEY_PUBLIC,    // public
    SCF_LEX_WORD_KEY_PROTECTED, // protected

    // exctral declare
    SCF_LEX_WORD_KEY_CONST,     // const
    SCF_LEX_WORD_KEY_STATIC,    // static
    SCF_LEX_WORD_KEY_EXTERN,    // extern

    // co routine
    SCF_LEX_WORD_KEY_ASYNC,     // async
    SCF_LEX_WORD_KEY_AWAIT,     // await

    // const literal value
    SCF_LEX_WORD_CONST_CHAR, 
    SCF_LEX_WORD_CONST_STRING,
    SCF_LEX_WORD_CONST_INT,
    SCF_LEX_WORD_CONST_FLOAT,
    SCF_LEX_WORD_CONST_BOOL,

    // important key value
    SCF_LEX_WORD_KEY_NULLPTR,   // nullptr
    SCF_LEX_WORD_KEY_QUESTION,  // ?

    // identity, start of a-z, A-Z, may include 0-9, _
    SCF_LEX_WORD_ID,
};

typedef struct
{
    scf_list_t list;       //make it to be a list

    enum scf_lex_words  type;

    union
    {
        char            c;  // value for char
        scf_string_t*   s;  // value for string
        int             i;  // value for int
        float           f;  // value for float
        unsigned int    b;  // value foe bool
    }data;

    scf_string_t*   text;  // original text
    scf_string_t*   file;  // original code file name
    int             line;   // line in the file
    int             pos;    // position in the line

}scf_lex_word_t;

static inline int scf_lex_word_is_identity(scf_lex_word_t* w)
{
    return w->type >= SCF_LEX_WORD_ID;
}

static inline int scf_lex_word_is_operator(scf_lex_word_t* w)
{
    return (w->type >= SCF_LEX_WORD_PLUS 
         && w->type <= SCF_LEX_WORD_DOT)
         || w->type == SCF_LEX_WORD_KEY_SIZEOF;
}

static inline int scf_lex_word_is_const(scf_lex_word_t* w)
{
    return w->type >= SCF_LEX_WORD_CONST_CHAR
        && w->type <= SCF_LEX_WORD_CONST_BOOL;
}

static inline int scf_lex_word_is_basic_type(scf_lex_word_t* w)
{
    return w->type >= SCF_LEX_WORD_KEY_CHAR
        && w->type <= SCF_LEX_WORD_KEY_BOOL;
}

scf_lex_word_t* scf_lex_word_alloc(scf_string_t* file, int line, int pos, enum scf_lex_words type);
scf_lex_word_t* scf_lex_word_clone(scf_lex_word_t* w);
void            scf_lex_word_free(scf_lex_word_t* w);

#endif