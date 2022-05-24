#include "scf_lex.h"

static scf_lex_key_word_t key_words[] = 
{
    {"if",      SCF_LEX_WORD_KEY_IF},
    {"else",    SCF_LEX_WORD_KEY_ELSE},
    {"switch",  SCF_LEX_WORD_KEY_SWITCH},
    {"case",    SCF_LEX_WORD_KEY_CASE},
    {"default", SCF_LEX_WORD_KEY_DEFAULT},
    {"for",     SCF_LEX_WORD_KEY_FOR},
    {"in",      SCF_LEX_WORD_KEY_IN},
    {"while",   SCF_LEX_WORD_KEY_WHILE},
    {"break",   SCF_LEX_WORD_KEY_BREAK},
    {"continue",SCF_LEX_WORD_KEY_CONTINUE},
    {"return",  SCF_LEX_WORD_KEY_RETURN},
    {"goto",    SCF_LEX_WORD_KEY_GOTO},
    {"try",     SCF_LEX_WORD_KEY_TRY},
    {"throw",   SCF_LEX_WORD_KEY_THROW},
    {"catch",   SCF_LEX_WORD_KEY_CATCH},
    {"error",   SCF_LEX_WORD_KEY_ERROR},
    {"sizeof",  SCF_LEX_WORD_KEY_SIZEOF},
    {"void",    SCF_LEX_WORD_KEY_VOID},
    {"int",     SCF_LEX_WORD_KEY_INT},
    {"char",    SCF_LEX_WORD_KEY_CHAR},
    {"string",  SCF_LEX_WORD_KEY_STRING},
    {"float",   SCF_LEX_WORD_KEY_FLOAT},
    {"bool",    SCF_LEX_WORD_KEY_BOOL},
    {"class",   SCF_LEX_WORD_KEY_CLASS},
    {"union",   SCF_LEX_WORD_KEY_UNION},
    {"struct",  SCF_LEX_WORD_KEY_STRUCT},
    {"private", SCF_LEX_WORD_KEY_PRIVATE},
    {"protected",SCF_LEX_WORD_KEY_PROTECTED},
    {"const",   SCF_LEX_WORD_KEY_CONST},
    {"static",  SCF_LEX_WORD_KEY_STATIC},
    {"extern",  SCF_LEX_WORD_KEY_EXTERN},
    {"async",   SCF_LEX_WORD_KEY_ASYNC},
    {"await",   SCF_LEX_WORD_KEY_AWAIT},
    {"nullptr", SCF_LEX_WORD_KEY_NULLPTR},
};

static scf_lex_escape_char_t escape_chars[] =
{
    {'n','\n'},
    {'r','\r'},
    {'t','\t'},
    {'0','\0'},
};

scf_lex_op_t ops[]=
{
    {'*',   SCF_LEX_WORD_STAR,         {{'='}},            {{SCF_LEX_WORD_MUL_ASSIGN}},                                     {1,1}},
    {'/',   SCF_LEX_WORD_DIV,          {{'='}},            {{SCF_LEX_WORD_DIV_ASSIGN}},                                     {1,1}},
    {'%',   SCF_LEX_WORD_MOD,          {{'='}},            {{SCF_LEX_WORD_MOD_ASSIGN}},                                     {1,1}},
    {'=',   SCF_LEX_WORD_STAR,         {{'='}},            {{SCF_LEX_WORD_MUL_ASSIGN}},                                     {1,1}},
    {'&',   SCF_LEX_WORD_AND,          {{'&'},{'='}},      {{SCF_LEX_WORD_LOGIC_AND},{SCF_LEX_WORD_AND_ASSIGN}},            {2,2}},
    {'|',   SCF_LEX_WORD_OR,           {{'|'},{'='}},      {{SCF_LEX_WORD_LOGIC_OR},{SCF_LEX_WORD_OR_ASSIGN}},              {2,2}},
    {'!',   SCF_LEX_WORD_LOGIC_NOT,    {{'='}},            {{SCF_LEX_WORD_NE}},                                             {1,1}},
    {'^',   SCF_LEX_WORD_XOR,          {{'='}},            {{SCF_LEX_WORD_XOR_ASSIGN}},                                     {1,1}},
    {'~',   SCF_LEX_WORD_NOT,          {{}},               {{}},                                                            {0,0}},
    {'(',   SCF_LEX_WORD_LP,           {{}},               {{}},                                                            {0,0}},
    {')',   SCF_LEX_WORD_RP,           {{}},               {{}},                                                            {0,0}},
    {'[',   SCF_LEX_WORD_LS,           {{}},               {{}},                                                            {0,0}},
    {']',   SCF_LEX_WORD_RS,           {{}},               {{}},                                                            {0,0}},
    {'{',   SCF_LEX_WORD_LB,           {{}},               {{}},                                                            {0,0}},
    {'}',   SCF_LEX_WORD_RB,           {{}},               {{}},                                                            {0,0}},
    {',',   SCF_LEX_WORD_COMMA,        {{}},               {{}},                                                            {0,0}},
    {';',   SCF_LEX_WORD_SEMICOLON,    {{}},               {{}},                                                            {0,0}},
    {':',   SCF_LEX_WORD_COLON,        {{'='}},            {{SCF_LEX_WORD_COLON_ASSIGN}},                                   {1,1}}, // 老师要求
    {'<',   SCF_LEX_WORD_LT,           {{'<','='},{'='}},  {{SCF_LEX_WORD_SHL,SCF_LEX_WORD_SHL_ASSIGN},{SCF_LEX_WORD_LE}},  {2,2}},
    {'>',   SCF_LEX_WORD_RT,           {{'>','='},{'='}},  {{SCF_LEX_WORD_SHR,SCF_LEX_WORD_SHR_ASSIGN},{SCF_LEX_WORD_GE}},  {2,2}},
    {'?',   SCF_LEX_WORD_KEY_QUESTION, {{}},               {{}},                                                            {0,0}}, // 老师要求
    {'.',   SCF_LEX_WORD_DOT,          {{'.','.'}},        {{SCF_LEX_WORD_TO,SCF_LEX_WORD_VAR_ARGS}},                       {1,2}}  // 老师要求
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

    scf_lex_char_t* c = _lex_pop_char(lex);

    if(c->c == EOF){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos,SCF_LEX_WORD_EOF);
        w->text = scf_string_cstr("eof");
        
        *pword = w;
        free(c);
        return 0;
    }

    if('\n' == c->c || '\r' == c->c || '\t' == c->c || ' ' == c->c){

        //scf_lex_word_t* w =scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos,SCF_LEX_WORD_SPACE);
        //w->text = scf_string_cstr_len(" ",1);
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
                return _lex_op_ll1(lex, pword, c, ops[0].t, ops[0].c1, ops[0].t1, ops[0].n);

            case '/':
                return _lex_op_ll1(lex, pword, c, ops[1].t, ops[1].c1, ops[1].t1, ops[1].n);

            case '%':
                return _lex_op_ll1(lex, pword, c, ops[2].t, ops[2].c1, ops[2].t1, ops[2].n);
            
            case '=':
                return _lex_op_ll1(lex, pword, c, ops[3].t, ops[3].c1, ops[3].t1, ops[3].n);

            case '&':
                return _lex_op_ll1(lex, pword, c, ops[4].t, ops[4].c1, ops[4].t1, ops[4].n);

            case '|':
                return _lex_op_ll1(lex, pword, c, ops[5].t, ops[5].c1, ops[5].t1, ops[5].n);

            case '!':
                return _lex_op_ll1(lex, pword, c, ops[6].t, ops[6].c1, ops[6].t1, ops[6].n);    
            
            case '^':
                return _lex_op_ll1(lex, pword, c, ops[7].t, ops[7].c1, ops[7].t1, ops[7].n);

            case '~':
                return _lex_op_ll1(lex, pword, c, ops[8].t, ops[8].c1, ops[8].t1, ops[8].n);

            case '(':
                return _lex_op_ll1(lex, pword, c, ops[9].t, ops[9].c1, ops[9].t1, ops[9].n);

            case ')':
                return _lex_op_ll1(lex, pword, c, ops[10].t, ops[10].c1, ops[10].t1, ops[10].n);

            case '[':
                return _lex_op_ll1(lex, pword, c, ops[11].t, ops[11].c1, ops[11].t1, ops[11].n);

            case ']':
                return _lex_op_ll1(lex, pword, c, ops[12].t, ops[12].c1, ops[12].t1, ops[12].n);

            case '{':
                return _lex_op_ll1(lex, pword, c, ops[13].t, ops[13].c1, ops[13].t1, ops[13].n);

            case '}':
                return _lex_op_ll1(lex, pword, c, ops[14].t, ops[14].c1, ops[14].t1, ops[14].n);

            case ',':
                return _lex_op_ll1(lex, pword, c, ops[15].t, ops[15].c1, ops[15].t1, ops[15].n);

            case ';':
                return _lex_op_ll1(lex, pword, c, ops[16].t, ops[16].c1, ops[16].t1, ops[16].n);

            case ':':
                return _lex_op_ll1(lex, pword, c, ops[17].t, ops[17].c1, ops[17].t1, ops[17].n);

            case '<':
                return _lex_op_ll1(lex, pword, c, ops[18].t, ops[18].c1, ops[18].t1, ops[18].n);

            case '>':
                return _lex_op_ll1(lex, pword, c, ops[19].t, ops[19].c1, ops[19].t1, ops[19].n);
            
            case '?':
                return _lex_op_ll1(lex, pword, c, ops[20].t, ops[20].c1, ops[20].t1, ops[20].n);

            case '.':
                return _lex_op_ll1(lex, pword, c, ops[21].t, ops[21].c1, ops[21].t1, ops[21].n);

            case '\'':
                return _lex_char(lex, pword, c);

            case '\"':
                return _lex_string(lex, pword, c);

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

    scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
    e->message = scf_string_cstr("invalid symbol!");
    scf_list_add_tail(&lex->error_list_head, &e->list);
    lex->read_pos++;
    
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
        scf_list_t*     l = scf_list_head(&lex->char_list_head);
        scf_lex_char_t* c = scf_list_data(l, scf_lex_char_t, list);

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
            w->text = scf_string_cstr("++");
            *pword = w;

            _lex_push_char(lex, c_gap_one); 
        
            // 记录错误，返回错误
            scf_lex_error_t* e =scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
            e->message=scf_string_cstr("error: \'+++\' is not suggest!" );
            scf_list_add_tail(&lex->error_list_head, &e->list);
            
            lex->read_pos += 2;
            c_gap_one = NULL;
            return -1;
        
        }
        else{

            scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_INC);
            w->text = scf_string_cstr("++");
            *pword = w;
            
            _lex_push_char(lex, c_gap_one);
        
            lex->read_pos += 2;
            c_gap_one = NULL;
        }

    }
    else if(c_next->c == '='){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_ADD_ASSIGN);
        w->text = scf_string_cstr("+=");
        *pword = w;


        lex->read_pos += 2;
        *pword = w;
    }else{
        _lex_push_char(lex, c_next);

        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_PLUS);
        w->text = scf_string_cstr("+");

        lex->read_pos++;
        *pword = w;
    }

    free(c);
    c_next = NULL;
    c = NULL;
    return 0;
}

static int _lex_minus(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c)
{
    assert(lex);
    assert(pword);

    scf_lex_char_t* c_next = _lex_pop_char(lex);
    assert(c_next);

    if(c_next->c == '-')
    {
        scf_lex_char_t* c_gap_one = _lex_pop_char(lex);
        assert(c_gap_one);

        if(c_gap_one->c == '-')
        {
            // 匹配为 "--"，将第三个 '-' 放入 lec->char_list_head
            scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_DEC);
            w->text = scf_string_cstr("--");
            *pword = w;

            _lex_push_char(lex, c_gap_one); 
        
            // 记录错误，返回错误
            scf_lex_error_t* e =scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
            e->message=scf_string_cstr("error: \'---\' is not suggest!" );
            scf_list_add_tail(&lex->error_list_head, &e->list);
            
            lex->read_pos += 2;
            c_gap_one = NULL;
            return -1;
        
        }
        else{

            scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_INC);
            w->text = scf_string_cstr("--");
            *pword = w;
            
            _lex_push_char(lex, c_gap_one);
        
            lex->read_pos += 2;
            c_gap_one = NULL;

            return 0;
        }

    }
    else if(c_next->c == '='){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_SUB_ASSIGN);
        w->text = scf_string_cstr("-=");
        *pword = w;


        lex->read_pos += 2;
        *pword = w;
    }    else if(c_next->c == '>'){
        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_ARROW);
        w->text = scf_string_cstr("->");
        *pword = w;


        lex->read_pos += 2;
        *pword = w;
    }else{
        _lex_push_char(lex, c_next);

        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_MINUS);
        w->text = scf_string_cstr("-");

        lex->read_pos++;
        *pword = w;
    }

    free(c);
    c_next = NULL;
    c = NULL;
    return 0;
}

static int _lex_op_ll1(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c, enum scf_lex_words t,
                        char c1[2][2],enum scf_lex_words t1[2][2], int n[2])
{
    assert(lex);
    assert(pword);
    assert(c);
    assert(c1);
    assert(t1);
    assert(n);

    scf_lex_word_t* w;
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
                    
                    if(j==n[1]-1||c1[i][j+1]=='\0'){
                        flag=1;
                        i--;
                        j++;
                        break;
                    }
                }
                else if(j>0)
                {
                    flag=1;
                    i--;
                    _lex_push_char(lex, c_next);
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

    if(flag==1)
    {
        w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, t1[i][j-1]);
        assert(w);

        w->text = scf_string_cstr_len((char*)(&c->c),1);
        for(int k=0;k<j;k++){
            scf_string_cat_cstr_len(w->text, (char*)(&c1[i][k]),1);
        }

        *pword = w;
        lex->read_pos += w->text->len;
        return 0;
    }
    else
    {
        w = scf_lex_word_alloc(lex->file, lex->read_lines,lex->read_pos, t);
        assert(w);

        w->text = scf_string_alloc();
        scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);
        
        *pword = w;
        lex->read_pos++;
    }

    return 0;
}


static int _lex_char(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c){
    scf_lex_word_t* w = NULL;
    scf_string_t* s = scf_string_cstr_len((char*)(&c->c),1);

    scf_lex_char_t* c1 =_lex_pop_char(lex);
    if(c1->c == '\\'){
        scf_lex_char_t* c2 = _lex_pop_char(lex);
        scf_lex_char_t* c3 = _lex_pop_char(lex);
        if(c3->c == '\''){
            scf_string_cat_cstr_len(s,(char*)&(c1->c),1);
            scf_string_cat_cstr_len(s,(char*)&(c2->c),1);
            scf_string_cat_cstr_len(s,(char*)&(c3->c),1);

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

static int _lex_string(scf_lex_t* t, scf_lex_word_t** pword, scf_lex_char_t* c)
{
    return -1;
}

static int  _lex_number(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c)
{
    assert(lex);
    assert(pword);
    assert(c);

    //首字符检查
    if( c->c  != '0' ){
        // 10 进制

        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_CONST_INT);
        assert(w);
        w->text = scf_string_cstr_len(((char*)&(c->c)), 1);
        
        free(c);
        c = NULL;

        c = _lex_pop_char(lex);
        assert(c);

        while(isdigit(c->c)){
            //整数部分
            scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);

            free(c);
            c = NULL;
            c = _lex_pop_char(lex);
            assert(c);
        }

        if(c->c == '.'){
            // 小数部分
            scf_lex_char_t* c_temp=_lex_pop_char(lex);

            // 检测是否为 ".." : SCF_LEX_WORD_TO
            if(c_temp->c == '.'){
                // 不是小数
                _lex_push_char(lex,c_temp);
                _lex_push_char(lex,c);
                c_temp = NULL;
                c = NULL;
                // 越界检测
                if(!_is_overflow(w,10)){
                    // 没越界返回整数部分
                    lex->read_pos += w->text->len;
                    *pword = w;
                    return 0;
                }
                else{
                    // 越界返回错误
                    scf_lex_word_free(w);
                    scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                    e->message = scf_string_cstr("int overflow!");
                    scf_list_add_tail(&lex->error_list_head, &e->list);
                    lex->read_pos += (w->text->len);
                    return -1;
                }

                return 0;
            }
            _lex_push_char(lex,c_temp);
            c_temp=NULL;

            w->type = SCF_LEX_WORD_KEY_FLOAT;
            scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);

            free(c);
            c = NULL;

            c = _lex_pop_char(lex);
            assert(c);

            while(isdigit(c->c)){
                scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);

                free(c);
                c = NULL;
                c = _lex_pop_char(lex);
                assert(c);
            }
        }    

        if(c->c == 'e'){
            //指数部分
            scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);

            free(c);
            c = NULL;

            c = _lex_pop_char(lex);
            assert(c);

            while(isdigit(c->c)){
                scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);

                free(c);
                c = NULL;

                c = _lex_pop_char(lex);
                assert(c);
            }

        }
        //最后一个 非10进制类整数/浮点/科学记数法符号 压栈
        _lex_push_char(lex,c);

        //溢出检查
        if(!_is_overflow(w,10)){
            //未溢出
            
            *pword = w;
            lex->read_pos += w->text->len;
            return 0;
        }
        else{
            //溢出
            lex->read_pos += w->text->len;
            return -1;

        }
    } else {
        // 首字符为 0

        free(c);
        c = NULL;

        scf_lex_word_t* w = scf_lex_word_alloc(lex->file, lex->read_lines, lex->read_pos, SCF_LEX_WORD_CONST_INT);
        assert(w);
        w->text = scf_string_cstr("0");

        c = _lex_pop_char(lex);
        assert(c);

        // 第二字符检测
        if( c->c == 'x' ){
            //16 进制

            free(c);
            c = NULL;

            scf_string_cat_cstr(w->text,"x");

            c = _lex_pop_char(lex);
            assert(c);
            
            if(isxdigit(c->c)){

                while(isxdigit(c->c)){
                    //是16进制字符
                    scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);

                    free(c);
                    c = NULL;
                    c = _lex_pop_char(lex);
                    assert(c);
                }

                if( !isxdigit(c->c) && (c->c !='$') ){
                    // 遇到操作符
                    _lex_push_char(lex,c);
                    c = NULL;
                    
                    // 越界检查
                    if(!_is_overflow(w,16)){
                        //没越界
                        lex->read_pos += (w->text->len);
                        *pword = w;
                        return 0;
                    } else {
                        //越界
                        scf_lex_word_free(w);
                        scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                        e->message = scf_string_cstr("int overflow!");
                        scf_list_add_tail(&lex->error_list_head, &e->list);
                        lex->read_pos += (w->text->len);
                        return -1;
                    }
                } else {
                    // 前缀正常，出现非16进制字母
                    
                    // 吞掉后面的字母数字串
                    lex->read_pos += (w->text->len);
                    scf_lex_word_free(w);
                    while(isalnum(c->c)){
                        free(c);
                        c=_lex_pop_char(lex);
                        lex->read_pos++;
                    }
                    _lex_push_char(lex,c);
                    
                    // 错误记录
                    scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                    e->message = scf_string_cstr("invalid hex: need a \' \' !");
                    scf_list_add_tail(&lex->error_list_head, &e->list);
                    return -1;
                }
            } else {
                // 0x后直接出现出现非16进制字母

                // 吞掉字母数字串
                lex->read_pos += (w->text->len);
                scf_lex_word_free(w);
                while(isalnum(c->c)){
                    free(c);
                    c=_lex_pop_char(lex);
                    lex->read_pos++;
                }
                _lex_push_char(lex,c);

                // 错误记录
                scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                e->message = scf_string_cstr("invalid hex: hex number include only 0~9 and a~f"); 
                scf_list_add_tail(&lex->error_list_head,&e->list);
                return -1;
            }


        } else if( c->c >= '0' && c->c <= '7' ) {
            //8 进制            
            while( c->c >= '0' && c->c <= '7' ){
                scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);

                free(c);
                c = NULL;

                c = _lex_pop_char(lex);
                assert(c);
            }

            if( !isalpha(c->c) && (c->c !='$') && c->c!='8' && c->c != '9'){
                // 遇到操作符
                _lex_push_char(lex,c);
                c = NULL;
                    
                // 越界检查
                if(!_is_overflow(w,8)){
                    //没越界
                    lex->read_pos += (w->text->len);
                    *pword = w;
                    return 0;
                } else {
                    //越界
                    scf_lex_word_free(w);
                    scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                    e->message = scf_string_cstr("int overflow!");
                    scf_list_add_tail(&lex->error_list_head, &e->list);
                    lex->read_pos += (w->text->len);
                    return -1;
                }
            } else {
                // 后缀出现 非8进制 符号

                // 吞掉字母数字串
                scf_lex_word_free(w);
                lex->read_pos += (w->text->len);
                while(isalnum(c->c)){
                    free(c);
                    c=_lex_pop_char(lex);
                    lex->read_pos++;
                }
                _lex_push_char(lex,c);

                scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                e->message = scf_string_cstr("invalid oct: need a \' \' !");
                scf_list_add_tail(&lex->error_list_head, &e->list);
                return -1;
            }
        }
        else if(c->c=='.'){
            // 0.开头的小数
            w->type = SCF_LEX_WORD_CONST_FLOAT;
            
            // 检测是否为 ".." : SCF_LEX_WORD_TO
            scf_lex_char_t* c1=_lex_pop_char(lex);
            if(c1->c == '.'){
                // 不是小数
                _lex_push_char(lex,c1);
                _lex_push_char(lex,c);
                c1 = NULL;
                c = NULL;
                // 越界检测
                if(!_is_overflow(w,10)){
                    // 没越界返回整数部分
                    lex->read_pos += (w->text->len);
                    *pword = w;
                    return 0;
                }
                else{
                    // 越界返回错误
                    scf_lex_word_free(w);
                    scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                    e->message = scf_string_cstr("float overflow!");
                    scf_list_add_tail(&lex->error_list_head, &e->list);
                    lex->read_pos += (w->text->len);
                    return -1;
                }
            }
            _lex_push_char(lex,c1);
            c1=NULL;

            scf_string_cat_cstr_len(w->text, ".", 1);

            free(c);
            c = NULL;

            c = _lex_pop_char(lex);
            assert(c);
            while(isdigit(c->c)){
                scf_string_cat_cstr_len(w->text, (char*)(&(c->c)), 1);
                free(c);
                c = NULL;
                c = _lex_pop_char(lex);
                assert(c);
            }
            //最后一个 非10进制类整数/浮点/科学记数法符号 压栈
            if(!isalpha(c->c) && c->c!='&'){
                _lex_push_char(lex,c);

                //溢出检查
                if(!_is_overflow(w,10)){
                    //未溢出
                    lex->read_pos += (w->text->len);
                    *pword = w;
                    return 0;
                }
                else{
                    //越界
                    scf_lex_word_free(w);
                    scf_lex_error_t* e = scf_lex_error_alloc(lex->file, lex->read_lines, lex->read_pos);
                    e->message = scf_string_cstr("float overflow!");
                    scf_list_add_tail(&lex->error_list_head, &e->list);
                    lex->read_pos += (w->text->len);
                    return -1;
                }
            }
        }    
    }
}


static int  _lex_identity(scf_lex_t* lex, scf_lex_word_t** pword, scf_lex_char_t* c)
{
    scf_string_t* s =scf_string_cstr_len((char*)(&c->c), 1);
    //lex->read_pos++;

    free(c);
    c=NULL;

    while(1){
        scf_lex_char_t* c1 = _lex_pop_char(lex);
        if( isalnum(c1->c) || c1->c == '_'){
            scf_string_cat_cstr_len(s, (char*)(&c1->c), 1);
            //lex->read_pos++;
            free(c1);
            c1=NULL;
        } else if(c1->c =='$') {
            lex->read_pos += s->len;
            scf_string_free(s);

            scf_lex_error_t* e = scf_lex_error_alloc(lex->file,lex->read_lines,lex->read_pos);
            e->message = scf_string_cstr("\'$\' is invalid char in identity!");
            scf_list_add_tail(&lex->error_list_head,&e->list);

            lex->read_pos++;
            c1 = _lex_pop_char(lex);
            while(isalnum(c1->c) || c1->c == '_' || c1->c == '$'){
                lex->read_pos++;
                c1 = _lex_pop_char(lex);
            }
            

            _lex_push_char(lex, c1);
            return -1;
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
            lex->read_pos += w->text->len;
            
            *pword = w;
            return 0;
 
        }
   }
}
int _is_overflow(scf_lex_word_t* w,int base)
{
    
    return 0;
}