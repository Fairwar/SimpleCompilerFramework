#include "./scf_lex/scf_lex.h"
#include "./scf_ast/scf_paser.h"
using namespace std;

int main(){
    scf_lex_t** plex = (scf_lex_t**)malloc(sizeof(scf_lex_t*));
    scf_lex_word_t** pword = (scf_lex_word_t**)malloc(sizeof(scf_lex_word_t*));
    int state =0;
    char* path = "./test.txt";

    if(scf_lex_open(plex,path)==-1){
        printf("文件打开失败\n");
        abort();
    }

    int f = scf_lex_pop_word(*plex,pword);
    
    stack<int>state;
    
    while(f == -1||(*pword)->type!=SCF_LEX_WORD_EOF){

    }
    scf_lex_close(*plex);
    system("pause");
    return 0;
}