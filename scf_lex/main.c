#include "scf_lex.h"

int main(){
    scf_lex_t** plex = malloc(sizeof(scf_lex_t*));
    scf_lex_word_t** pword = malloc(sizeof(scf_lex_word_t*));

    char* path = "test.txt";

    if(scf_lex_open(plex,path)==-1){
        printf("文件打开失败\n");
    }

    scf_lex_pop_word(*plex,pword);
    while((*pword)->type!=SCF_LEX_WORD_EOF){
        if((*pword)->type == SCF_LEX_WORD_ID){
            printf(" < id , %s >\n",(char*)((*pword)->text->data));
        }
        else{
            printf(" < %s >\n",(char*)((*pword)->text->data));
        }
        scf_lex_pop_word(*plex,pword);
    }
    scf_lex_close(*plex);
    system("pause");
    return 0;
}