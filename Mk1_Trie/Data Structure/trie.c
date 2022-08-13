#include <stdlib.h>
#include <stdio.h>
#define DIM 64 // ADMISSIBLE LETTERS a-z, A-Z, -,_ TOTAL 64 CHARS
#define INTERNAL 0
#define EOW 1
#define STARTNUM '0'
#define ENDNUM '9'
#define LETT_START 'a'
#define LETT_END 'z'
#define LETTSTART 'A'
#define LETTEND 'Z'

typedef struct trie_node_s{
    struct trie_node_s * sons[DIM];
    int8_t state; //1 means END OF WORD 0 means INTERNAL LETTER 2 MEANS EOW BUT WORD IS NOT COMPATIBLE 
    int nsons;
}node_t;



int findword(node_t *, char[]);
int toindex(char);
char toletter(int);
void addword(node_t * , char []);
void printwords(node_t *, int);
void printer(node_t * , int , int , char[]);
void deleteword(node_t *, char[]);
void deletetrie(node_t *);
node_t * createTrie();
int main(int argc, char * argv[]){

    node_t * head;
    char temp[10];
    int8_t useless __attribute__((unused));


    head = createTrie();
    for(int i=0; i < 10; i++){
        useless = scanf("%s", temp);
        addword(head, temp);
    }
    printf("List of Words\n");
    printwords(head, 9);
    useless = scanf("%s", temp);
    deleteword(head, temp);
    printf("List of Words Updated\n");
    printwords(head, 9);
    return 0;
}

node_t * createTrie(){
    node_t * tmp;
    tmp = malloc(sizeof(node_t));
    if(tmp != NULL){
        tmp->state=INTERNAL;
        tmp->nsons=0;
        for(int i=0; i < DIM; i++){
            tmp->sons[i]=NULL;
        }
    }
    return tmp;
}

void addword(node_t * head, char toinsert []){
    int i;
    int pos;
    for(i=0; toinsert[i]!= '\0'; i++){
        pos = toindex(toinsert[i]);
        if(head->sons[pos]==NULL){
            head->sons[pos] = createTrie();
        }
        head->nsons += 1;
        head = head->sons[pos];
    }
    head->state = EOW;
    return;
}
void printwords(node_t * head, int dimmax){

    char * wordtemp;
    wordtemp = malloc((dimmax+1) * sizeof(char));
    if(wordtemp != NULL){
        printer(head, 0, dimmax, wordtemp);
        free(wordtemp);
    }
    return;  
}
int findword(node_t * head, char word[]){
    node_t * tmp;
    for(int i=0; word[i] != '\0';i++){
        tmp = head->sons[toindex(word[i])];
        if(tmp == NULL)
            return 0;
        head = tmp;
    }
    if(head->state == EOW)
        return 1;
    return 0;
}
void printer(node_t * elem, int pos, int max, char word[]){
    for(int i=0; i < DIM; i++){
        if(elem->sons[i] != NULL && pos <= max){
            word[pos]= toletter(i);
            printer(elem->sons[i], pos+1, max, word);
        }
    }
    if(elem->state == EOW){
        word[pos]='\0';
        printf("%s\n", word);
    }
}
void deleteword(node_t * head, char tobedeleted[]){
    node_t * tmp;
    int pos;
    if(findword(head, tobedeleted)==1){
        /*for(int i=0; tobedeleted[i] != '\0'; i++){
            printf("Guardo lettera %c\n", tobedeleted[i]);
            printf("Ho %d figli da questo nodo\n", head->nsons);
            head->nsons -= 1;
            //if(head->nsons == 0){
            //    tmp = head;
            //    free(tmp);
            //}
            head = head->sons[toindex(tobedeleted[i])];
        }
        if(head != NULL){
            head->state = INTERNAL;
        }*/
        for(int i=0; tobedeleted[i] != '\0'; i++){
            pos = toindex(tobedeleted[i]);
            head->nsons -= 1;
            tmp = head->sons[pos];
            if(tmp->nsons == 1){  
                deletetrie(tmp);
                head->sons[pos] = NULL;
                return;
            }
            head = tmp;
        }
        if(head != NULL){
            head->state = INTERNAL;
        }
    }
    return;
}


int toindex(char letter){
    
    
    // - 0
    //0-9 1-10
    //A-Z 11-36
    // _ 37
    //a-z 38-63   
    if(letter == '-')
        return 0;

    if(letter <= ENDNUM)
        return letter-STARTNUM +1;

    if(letter <= LETTEND)
        return letter-LETTSTART + 11;

    if(letter == '_')
        return 37;

    return letter-LETT_START + 38;

}
char toletter(int pos){
    // - 0
    //0-9 1-10
    //A-Z 11-36
    // _ 37
    //a-z 38-63

    if(pos == 0)
        return '-';
    if(pos < 11)
        return (pos-1)+STARTNUM;
    if(pos < 37)
        return (pos-11)+LETTSTART;
    if(pos == 37)
        return '_';
    return (pos-38)+LETT_START;
}

void deletetrie(node_t * head){
    for(int i=0; i < DIM; i++){
        if(head->sons[i] != NULL){
            deletetrie(head->sons[i]);
        }
    }
    free(head);
    return;
}


void resettrie(node_t * head){
    for(int i=0; i < DIM; i++){
        if(head->sons[i] != NULL){
            resettrie(head->sons[i]);
        }
    }
    if(head->state == 2)
        head->state = 1;
    return;
}

