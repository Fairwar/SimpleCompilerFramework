#include "./scf_ast/scf_paser.h"
using namespace std;

int main(){
    scf_lex_t** plex = (scf_lex_t**)malloc(sizeof(scf_lex_t*));
    scf_lex_word_t** pword = (scf_lex_word_t**)malloc(sizeof(scf_lex_word_t*));

    char* path = "./test.txt";

    if(scf_lex_open(plex,path)==-1){
        printf("文件打开失败\n");
        abort();
    }

    int f = scf_lex_pop_word(*plex,pword);
    
    stack<int> state_stack;
    state_stack.push(0);
    
    while(f == -1||(*pword)->type!=SCF_LEX_WORD_EOF){
        
        if(state_stack.top()==0){
            if((*pword)->type==SCF_LEX_WORD_LP){
                state_stack.push(14);
            }
            else if((*pword)->type==SCF_LEX_WORD_ID){
                state_stack.push(12);
            }
            else if(scf_lex_word_is_const(*pword)){
                state_stack.push(13);
            }
            else{
                printf("error!\n");
            }
        }

        if(state_stack.top()==0){
            if((*pword)->type==SCF_LEX_WORD_LP){
                state_stack.push(14);
            }
            else if((*pword)->type==SCF_LEX_WORD_ID){
                state_stack.push(12);
            }
            else if(scf_lex_word_is_const(*pword)){
                state_stack.push(13);
            }
            else{
                printf("error!\n");
            }
        }
    
    }
    
    
    scf_lex_close(*plex);
    system("pause");
    return 0;
}