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
        printf("<%s>",(*pword)->text);
        system("pause");
    }
    scf_lex_close(*plex);
    return 0;
}