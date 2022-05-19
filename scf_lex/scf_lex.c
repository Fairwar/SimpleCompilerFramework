#include "scf_lex.h"

static scf_lex_key_word_t key_words[] = 
{
    {"if",      SCF_LEX_WORD_KEY_IF},
    {"else",    SCF_LEX_WORD_KEY_ELSE}
};

static scf_lex_escape_char_t escape_chars[] =
{
    {'n','\n'},
    {'r','\r'},
    {'t','\t'},
    {'0','\0'},
};

int scf_lex_open(scf_lex_t** plex, const char* path)
{
    printf("%s(),%d, keywords: %ld\n", __func__, __LINE__,
            sizeof(key_words)/sizeof(key_words[0]));
    
    assert(plex);
    assert(path);

    scf_lex_t* lex = calloc(1,sizeof(scf_lex_t));
    assert(lex);

    scf_list_init(&lex->word_list_head);
    scf_list_init(&lex->error_list_head);
    scf_list_init(&lex->char_list_head);

    lex->fp = fopen(path,"r");
    if(!lex->fp){
        printf("%s(),%d\n", __func__, __LINE__);

        free(lex);
        lex=NULL;
        return -1;    
    }
    lex->file = scf_string_cstr(path);
    assert(lex->file);

    *plex = lex;
    return 0;
}

int scf_lex_close(scf_lex_t* lex)
{
    assert(lex);

    scf_list_clear(&lex->word_list_head,scf_lex_word_t,list,scf_lex_word_free);
    scf_list_clear(&lex->error_list_head,scf_lex_error_t,list,scf_lex_error_free);
    
    free(lex);
    lex = NULL;
    
    return 0;
}

int scf_lex_push_word(scf_lex_t* lex, scf_lex_word_t* word)
{
    assert(lex);
    assert(word);

    scf_list_add_front(&lex->word_list_head, &word->list);
    return 0;
}

static scf_lex_char_t* scf_lex_pop_char(scf_lex_t* lex)
{
    assert(lex);
    assert(lex->fp);

    if(!scf_list_empty(&lex->char_list_head)){
        scf_list_t*     l = scf_list_head(&lex->word_list_head);
        scf_lex_char_t* c = scf_list_data(l, scf_lex_word_t, list);

        scf_list_del(&c->list);
        return c;
    }

    scf_lex_char_t* c = malloc(sizeof(scf_lex_char_t));
    assert(c);

    c->c = tolower(fgetc(lex->fp));
}

static void scf_lex_push_char(scf_lex_t* lex, scf_lex_char_t*c)
{
    assert(lex);
    assert(c);

    scf_list_add_front(&lex->char_list_head, &c->list);
}

static void scf_lex_jump_space(scf_lex_t* lex)
{
    scf_lex_char_t* c = scf_lex_pop_char(lex);

    if('\n' == c->c || '\r' == c->c || '\t' == c->c || ' ' == c->c){
        if('\n' == c->c){
            lex->read_lines++;
            lex->read_pos = 0;
        }else{
            lex->read_pos++;
        }

        free(c);
        c = NULL;
        scf_lex_jump_space(lex);
    }else{
        scf_lex_push_char(lex,c);
    }
}

int scf_lex_pop_word(scf_lex_t* lex, scf_lex_word_t** pword)
{
    assert(lex);
    assert(pword);
    assert(lex->fp);

    if(!scf_list_empty(&lex->word_list_head)){
        scf_list_t*     l = scf_list_head(&lex->word_list_head);
        scf_lex_word_t* w = scf_list_data(l, scf_lex_word_t, list);

        scf_list_del(&w->list);
        *pword = w;
        return 0;
    }

    scf_lex_char_t* c= scf_lex_pop_char(lex);
    if(c->c == EOF){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos,SCF_LEX_WORD_EOF);
        w->text = scf_string_cstr("eof");
        
        *pword = w;
        free(c);
        return 0;
    }

    if('\n' == c->c || '\r' == c->c || '\t' == c->c || ' ' == c->c){

        scf_lex_word_t* w =scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos,SCF_LEX_WORD_SPACE);
        w->text = scf_string_cstr(c->c);
        *pword =w;

        if('\n' == c->c){
            lex->read_lines++;
            lex->read_pos = 0;
        }else{
            lex->read_pos++;
        }

        free(c);
        c = NULL;
        scf_lex_jump_space(lex);
        return scf_lex_pop_word(lex, pword);
    }

    switch(c->c){
        case '+':
            return _lex_plus(lex, pword, c);
        case '-':
            return _lex_minus(lex, pword, c);
        case '*':
            char c1='=';
            enum scf_lex_words t1 = SCF_LEX_WORD_MUL_ASSIGN;
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_STAR, &c1,  &t1, 1);
        case '/':
            char c1='=';
            enum scf_lex_words  t1 = SCF_LEX_WORD_DIV_ASSIGN;
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_DIV, &c1,  &t1, 1);
        case '%':
            char c1='=';
            enum scf_lex_words  t1 = SCF_LEX_WORD_MOD_ASSIGN;
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_MOD, &c1,  &t1, 1);
        case '=':
            char c1='=';
            enum scf_lex_words  t1 = SCF_LEX_WORD_EQ;
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_ASSIGN, &c1,  &t1, 1);
        case '&':
            char c1={'&','='};
            enum scf_lex_words t1 ={
                SCF_LEX_WORD_LOGIC_AND,
                SCF_LEX_WORD_AND_ASSIGN
            };
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_AND, &c1, &t1,2);
        case '|':
            char c1={'|','='};
            enum scf_lex_words t1 ={
                SCF_LEX_WORD_LOGIC_OR,
                SCF_LEX_WORD_OR_ASSIGN
            };
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_OR, &c1, &t1,2);
        case '!':
            char c1='=';
            enum scf_lex_words  t1 = SCF_LEX_WORD_NE;
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_LOGIC_NOT, &c1,  &t1, 1);
        case '^':
            char c1='=';
            enum scf_lex_words  t1 = SCF_LEX_WORD_XOR_ASSIGN;
            return _lex_op2_ll1(lex, pword, c, SCF_LEX_WORD_XOR, &c1,  &t1, 1);
        case '~':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_NOT);
        case '(':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_LP);
        case ')':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_RP);
        case '[':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_LS);
        case ']':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_RS);
        case '{':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_LB);
        case '}':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_RB);
        case ',':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_COMMA);
        case ';':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_SEMICOLON);
        case ':':
            return _lex_op1_ll1(lex, pword, c,SCF_LEX_WORD_COLON);
        case '<':
            return _lex_op3_ll1(lex, pword, c, '=', '>', '=',
                    SCF_LEX_WORD_LT, SCF_LEX_WORD_LE, SCF_LEX_WORD_SHL ,SCF_LEX_WORD_SHL_ASSIGN);
        case '>':
            return _lex_op3_ll1(lex, pword, c, '=', '>', '=',
                    SCF_LEX_WORD_RT, SCF_LEX_WORD_GE, SCF_LEX_WORD_SHR, SCF_LEX_WORD_SHR_ASSIGN);
        case '\'':
            return _lex_char(lex, pword, c);
        case '\"':
            return _lex_string(lex, pword, c);
        default:
            break;
    }
    
    if(isdigit(c->c))
    {
        return _lex_num(lex, pword, c);
    }

    if((c->c == '_')||isalpha(c->c)){
        return _lex_identity(lex, pword, c);
    }

    return -1;
}
