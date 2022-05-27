#include "./scf_lex/scf_lex.h"


int main(){
    scf_lex_t** plex = malloc(sizeof(scf_lex_t*));
    scf_lex_word_t** pword = malloc(sizeof(scf_lex_word_t*));

    char* path = "./test.txt";

    if(scf_lex_open(plex,path)==-1){
        printf("文件打开失败\n");
        abort();
    }

    int f = scf_lex_pop_word(*plex,pword);
    while(f == -1||(*pword)->type!=SCF_LEX_WORD_EOF){
        if(f == -1)
        {
            scf_list_t * l = &((*plex)->error_list_head);
            while(!scf_list_empty(l)){
                scf_list_t * h = scf_list_head(l);
                scf_lex_error_t* e = scf_list_data(h, scf_lex_error_t, list);
                scf_list_del(&e->list);
                printf("< file:\"%s\", line:%d, pos:%d, ERROR:%s>\n",
                         e->file->data, e->line+1, e->pos+1, e->message->data);
                scf_lex_error_free(e);
            }
        }
        else
        {
            if((*pword)->type >= SCF_LEX_WORD_ID){
                printf("< ID , %s >\n",(char*)((*pword)->text->data));
            }else if((*pword)->type == SCF_LEX_WORD_CONST_INT){
                printf("< INT , %d >\n",(*pword)->data.i);
            }else if((*pword)->type == SCF_LEX_WORD_CONST_FLOAT){
                printf("< FLOAT , %f >\n",(*pword)->data.f);
            }else if((*pword)->type == SCF_LEX_WORD_CONST_STRING){
                printf("< STRING , %s >\n",(*pword)->data.s->data);
            }else if((*pword)->type == SCF_LEX_WORD_CONST_CHAR){
                printf("< CHAR , %c >\n",(*pword)->data.c);
            }else if((*pword)->type == SCF_LEX_WORD_CONST_BOOL){
                printf("< BOOL , %s >\n",(*pword)->data.b?"true":"false");
            }else{
                printf("< %s >\n",((*pword)->text->data));
            }
            scf_lex_word_free(*pword);
        }
        f = scf_lex_pop_word(*plex,pword);
    }
    scf_lex_close(*plex);
    //system("pause");
    return 0;
}