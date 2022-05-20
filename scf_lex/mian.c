#include"scf_lex.h"
int main(){
    scf_lex_t** plex =malloc(sizeof(scf_lex_t*));
    scf_lex_word_t** pword = malloc(sizeof(scf_lex_word_t*));

    char* path = "./test.txt";
    scf_lex_open(plex,path);

    scf_lex_pop_word(*plex,pword);
    while((*pword)->type!=SCF_LEX_WORD_EOF){
        printf("<>");
    }
}