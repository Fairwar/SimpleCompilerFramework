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

    scf_lex_char_t* c= _lex_pop_char(lex);

    if(c->c == EOF){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos,SCF_LEX_WORD_EOF);
        w->text = scf_string_cstr("eof");
        
        *pword = w;
        free(c);
        return 0;
    }

    if('\n' == c->c || '\r' == c->c || '\t' == c->c || ' ' == c->c){

        //scf_lex_word_t* w =scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos,SCF_LEX_WORD_SPACE);
        //w->text = scf_string_cstr_len(' ');
        //*pword =w;

        if('\n' == c->c){
            lex->read_lines++;
            lex->read_pos = 0;
        }else{
            lex->read_pos++;
        }

        free(c);
        c = NULL;
        _lex_jump_space(lex);
        return scf_lex_pop_word(lex, pword);
    }

    switch(c->c){

        case '+':
            return _lex_plus(lex, pword, c);

        case '-':
            return _lex_minus(lex, pword, c);

        case '*':
            {   
                char c1[1][1] = {{'='}};
                enum scf_lex_words t1[1][1] = {{SCF_LEX_WORD_MUL_ASSIGN}};
                int n[2] = {1,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_STAR, c1,  t1, n);
            }
        case '/':
            {    char c1[1][1] = {{'='}};
                enum scf_lex_words  t1[1][1] = {{SCF_LEX_WORD_DIV_ASSIGN}};
                int n[2] = {1,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_DIV, c1,  t1, n);
            }
        case '%':
            {    char c1[1][1] = {{'='}};
                enum scf_lex_words  t1[1][1] = {{SCF_LEX_WORD_MOD_ASSIGN}};
                int n[2] = {1,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_MOD, c1,  t1, n);
            }
        case '=':
            {    char c1[1][1] = {{'='}};
                enum scf_lex_words  t1[1][1] = {{SCF_LEX_WORD_EQ}};
                int n[2] = {1,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_ASSIGN, c1,  t1, n);
            }
        case '&':
            {   char c1[2][1] = {{'&'},{'='}};
                enum scf_lex_words* t1[2][1] = {{SCF_LEX_WORD_LOGIC_AND}, {SCF_LEX_WORD_AND_ASSIGN}};
                int n[2]={2,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_AND, c1, t1,n);
            }

        case '|':
            {    char c1[2][1] = {{'|'},{'='}};
                enum scf_lex_words t1[2][1] = {{SCF_LEX_WORD_LOGIC_OR}, {SCF_LEX_WORD_OR_ASSIGN}};
                int n[2] = {2,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_OR, c1, t1,n);
            }    
        case '!':
            {    char c1[1][1] = {{'='}};
                enum scf_lex_words  t1[1][1] = {{SCF_LEX_WORD_NE}};
                int n[2]={1,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_LOGIC_NOT, c1, t1, n);
            }    
        case '^':
            {    char c1[1][1] = {{'='}};
                enum scf_lex_words  t1[1][1] = {{SCF_LEX_WORD_XOR_ASSIGN}};
                int n[2]={1,1};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_XOR, c1, t1, n);
            }    
        case '~':
            {    char c1[1][1] = {{}};
                enum scf_lex_words  t1[1][1] = {{}};
                int n[2]={0,0};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_NOT, c1, t1, n);
            }
        case '(':
            {    char c1[1][1] = {{}};
                enum scf_lex_words  t1[1][1] = {{}};
                int n[2]={0,0};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_LP, c1, t1, n);
            }    
        case ')':
            {    char c1[1][1] = {{}};
                enum scf_lex_words  t1[1][1] = {{}};
                int n[2]={0,0};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_RP, c1, t1, n);
            }    
        case '[':
            {    char c1[1][1] = {{}};
                enum scf_lex_words  t1[1][1] = {{}};
                int n[2]={0,0};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_LS, c1, t1, n);
            }    
        case ']':
            {    char c1[1][1] = {{}};
                enum scf_lex_words  t1[1][1] = {{}};
                int n[2]={0,0};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_RS, c1, t1, n);
            }    
        case '{':
            {    char c1[1][1] = {{}};
                enum scf_lex_words  t1[1][1] = {{}};
                int n[2]={0,0};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_LB, c1, t1, n);
            }    
        case '}':
            {
                char c1[1][1] = {{}};
                enum scf_lex_words  t1[1][1] = {{}};
                int n[2]={0,0};
                return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_RB, c1, t1, n);
            }
        case ',':
            {char c1[1][1] = {{}};
            enum scf_lex_words  t1[1][1] = {{}};
            int n[2]={0,0};
            return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_COMMA, c1, t1, n);}
        
        case ';':
            {char c1[1][1] = {{}};
            enum scf_lex_words  t1[1][1] = {{}};
            int n[2]={0,0};
            return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_SEMICOLON, c1, t1, n);}
        
        case ':':
            {char c1[1][1] = {{}};
            enum scf_lex_words  t1[1][1] = {{}};
            int n[2]={0,0};
            return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_COLON, c1, t1, n);}
        
        case '<':
            {char c1[2][2] = {{'=','\0'},{'<','='}};
            enum scf_lex_words t1[2][2] = { { SCF_LEX_WORD_LE,  SCF_LEX_WORD_LE },
                                            { SCF_LEX_WORD_SHL, SCF_LEX_WORD_SHL_ASSIGN } };
            int n[2] = {2, 2};
            return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_LT, c1, t1, n);}

        case '>':
            {char c1[2][2] = {{'=','\0'},{'>','='}};
            enum scf_lex_words t1[2][2] = { { SCF_LEX_WORD_GE,  SCF_LEX_WORD_GE },
                                            { SCF_LEX_WORD_SHR, SCF_LEX_WORD_SHR_ASSIGN } };
            int n[2]={2, 2};
            return _lex_op_ll1(lex, pword, c, SCF_LEX_WORD_LT, c1, t1, n);}

        //case '.':
            //return _lex_dot(lex, pword, c);

        case '\'':
            return _lex_char(lex, pword, c);

        //case '\"':
            //return _lex_string(lex, pword, c);

        default:
            break;
    }
    
    if(isdigit(c->c))
    {
        return _lex_number(lex, pword, c);
    }

    if((c->c == '_')||isalpha(c->c)){
        return _lex_identity(lex, pword, c);
    }

    return -1;
}

static void _lex_push_char(scf_lex_t* lex, scf_lex_char_t*c)
{
    assert(lex);
    assert(c);

    scf_list_add_front(&lex->char_list_head, &c->list);


}

static scf_lex_char_t* _lex_pop_char(scf_lex_t* lex)
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
    return c;
}

static void _lex_jump_space(scf_lex_t* lex)
{
    scf_lex_char_t* c = _lex_pop_char(lex);

    if('\n' == c->c || '\r' == c->c || '\t' == c->c || ' ' == c->c){
        if('\n' == c->c){
            lex->read_lines++;
            lex->read_pos = 0;
        }else{
            lex->read_pos++;
        }

        free(c);
        c = NULL;
        _lex_jump_space(lex);
    }else{
        _lex_push_char(lex,c);
    }
}

static int _find_key_word(const char* text)
{
    int i;
    for(i=0;i<sizeof(key_words)/sizeof(key_words[0]);i++){
        if(!strcmp(key_words[i].text, text)){
            return  key_words[i].type;
        }
    }
    return -1;
}

static int _find_escape_char(const char c){
    int i;
    for(i = 0; i < sizeof(escape_chars)/sizeof(escape_chars[0]); i++){
        if(escape_chars[i].origin == c){
            return escape_chars[i].escape;
        }
    }
    return c;
}

static int _lex_plus(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c)
{
    assert(lex);
    assert(pword);

    scf_lex_char_t* c_next = _lex_pop_char(lex);
    assert(c_next);

    if(c_next->c == '+')
    {
        scf_lex_char_t* c_gap_one = _lex_pop_char(lex);
        assert(c_gap_one);

        if(c_gap_one->c == '+')
        {
            // 匹配为 "++"，将第三个 '+' 放入 lec->char_list_head
            scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_INC);
            _lex_push_char(lex, c_gap_one); 
        
            // 记录错误，返回错误
            scf_lex_error_t* e =scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
            e->message=scf_string_cstr("error: \'+++\' is not suggest!" );
            scf_list_add_tail(&lex->error_list_head, &e->list);
            // return -1;
        
        }
        else{
            _lex_push_char(lex, c_gap_one);

            scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_INC);
            w->text = scf_string_cstr("++");
        }

        lex->read_pos += 2;
        free(c_gap_one);
        c_gap_one = NULL;

    }
    else if(c_next->c == '='){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_ADD_ASSIGN);
        w->text = scf_string_cstr("+=");

        lex->read_pos += 2;
        *pword = w;
    }else{
        _lex_push_char(lex, c_next);

        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_PLUS);
        w->text = scf_string_cstr("+");

        lex->read_pos++;
        *pword = w;
    }
    free(c_next);
    free(c);
    c_next = NULL;
    c = NULL;
    return 0;
}

static int _lex_minus(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c)
{
    assert(lex);
    assert(pword);
    assert(c);

    scf_lex_char_t* c_next = _lex_pop_char(lex);
    assert(c_next);

    if(c_next->c == '-')
    {
        scf_lex_char_t* c_gap_one = _lex_pop_char(lex);
        assert(c_gap_one);

        if(c_gap_one->c == '-')
        {
            // 匹配为 "++"，将第三个 '+' 放入 lec->char_list_head
            scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_DEC);
            _lex_push_char(lex, c_gap_one); 
        
            // 记录错误，返回错误
            scf_lex_error_t* e =scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
            e->message=scf_string_cstr("error: \'---\' is not suggest!" );
            scf_list_add_tail(&lex->error_list_head, &e->list);
            // return -1;
        
        }
        else {
            _lex_push_char(lex, c_gap_one);

            scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_DEC);
            w->text = scf_string_cstr("--");
        }

        lex->read_pos += 2;
        free(c_gap_one);
        c_gap_one = NULL;

    }
    else if(c_next->c == '='){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_SUB_ASSIGN);
        w->text = scf_string_cstr("-=");

        lex->read_pos += 2;
        *pword = w;
    }
    else if(c_next == '>'){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_ARROW);
        w->text = scf_string_cstr("->");

        lex->read_pos += 2;
        *pword = w;
    }
    else{
        _lex_push_char(lex, c_next);

        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_MINUS);
        w->text = scf_string_cstr("-");

        lex->read_pos++;
        *pword = w;
    }
    free(c_next);
    free(c);
    c_next = NULL;
    c = NULL;
    return 0;
}

static int _lex_op_ll1(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c,
                 enum scf_lex_words t, char** c1, enum scf_lex_words** t1, int* n)
{
    assert(lex);
    assert(pword);
    assert(c);
    assert(c1);
    assert(t1);
    assert(n);

    int i=0,j=0,flag=0;

    for(i=0; i<n[0] && !flag; i++){
        for(j=0; j<n[1] && !flag; j++){
            if(c1[i][j]!='\0')
            {
                scf_lex_char_t* c_next = _lex_pop_char(lex);
                assert(c_next);

                if(c1[i][j]==c_next->c){
                    free(c_next);
                    c_next = NULL;
                    
                    if(j==n[1]-1){
                        flag=1;
                        j++;
                        break;
                    }
                }
                else if(j>0)
                {
                    flag=1;
                    break;
                }
                else
                {
                    _lex_push_char(lex, c_next);
                    break;
                }
            }
            else{
                flag=1;
                break;
            }

        }
    }

    scf_lex_word_t* w;
    if(flag==1)
    {
        w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, t1[i][j]);
        assert(w);

        w->text = scf_string_alloc();
        for(int k=0;k<j;k++){
            scf_string_cat_cstr_len(w->text, (char*)(&c1[i][k]),1);
        }

        *pword = w;
        lex->read_pos += j+1;

    }
    else
    {
        w = scf_lex_word_alloc(lex->file, lex->read_lines,lex->read_pos, t);
        assert(w);

        w->text = scf_string_alloc();
        scf_string_cat_cstr_len(w->text, &c, 1);
        
        *pword = w;
        lex->read_pos++;
    }

    return 0;
}

static int  _lex_number(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c)
{
}

static int _lex_char(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c){
    scf_lex_word_t* w = NULL;
    scf_string_t* s = scf_string_cstr_len((char*)&c->c,1);

    scf_lex_char_t* c1 =_lex_pop_char(lex);
    if(c1->c == '\\'){
        scf_lex_char_t* c2 = _lex_pop_char(lex);
        scf_lex_char_t* c3 = _lex_pop_char(lex);
        if(c3->c == '\''){
            scf_string_cat_cstr_len(s,(char*)&c1->c,1);
            scf_string_cat_cstr_len(s,(char*)&c2->c,1);
            scf_string_cat_cstr_len(s,(char*)&c3->c,1);

            w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_CONST_CHAR);
            w->data.c = _find_escape_char(c2->c);
            lex->read_pos += 4;

            free(c3);
            c3 = NULL;
            free(c2);
            c2 = NULL;
        }
        else{
            _lex_push_char(lex,c3);
            _lex_push_char(lex,c2);

            scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
            e->message = scf_string_cstr("No matching \'\'\'");
            
            scf_list_add_tail(&lex->error_list_head,&e->list);
            return -1;
        }
    }
}

static int  _lex_identity(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c)
{
    scf_string_t* s =scf_string_cstr_len((char*)&c->c, 1);
    lex->read_pos++;

    free(c);
    
    c=NULL;

    while(1){
        scf_lex_char_t* c1 = _lex_pop_char(lex);
        if( isdigit(c1->c) || isalpha(c1->c) || c1->c == '_'){
            scf_string_cat_cstr_len(s, (char*)&c1->c, 1);
            lex->read_pos++;
            free(c1);
            c1=NULL;
        } 
        else {
            _lex_push_char(lex, c1);
            c1 = NULL;
            
            int type = _find_key_word(s->data);

            scf_lex_word_t* w = NULL;
            if(type == -1){
                type = SCF_LEX_WORD_ID + lex->num_id++;
                w = scf_lex_word_alloc(lex->file,lex->read_lines, lex->read_pos, type);
            }
            else{
                w = scf_lex_word_alloc(lex->file,lex->read_lines, lex->read_pos, type);
            }

            w->text = s;
            s =NULL;

            *pword = w;
            return 0;
        }
    }
}
