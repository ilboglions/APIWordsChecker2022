#include <stdio.h>
#include <stdlib.h>

#define NOTHING '#'
#define DIM 64 // ADMISSIBLE LETTERS a-z, A-Z, -,_ TOTAL 64 CHARS
#define INTERNAL 0
#define EOW 1
#define STARTNUM '0'
#define ENDNUM '9'
#define LETT_START 'a'
#define LETT_END 'z'
#define LETTSTART 'A'
#define LETTEND 'Z'
#define LENMAX 5 //WORDS WILL HAVE A MAXIMUM LENGTH OF 10^5 CHARS
#define COMMANDMAXLEN 17
#define COMMAND '+'
#define NEWGAME 0
#define PRINTREMAINING 1
#define ADDWORDS 2
#define ENDADD 3
#define INGAME 4
#define NOTFOUND "not_exists\0"
#define KO -1
#define GAMEOVER "ko\0"
#define WIN "ok\0"
#define RIGHT '+'
#define WRONG '/'
#define WRONGPOS '|'


typedef struct compatibility_s{
    int counters[DIM];
    int8_t definitive[DIM];
    int correctocc[DIM];
    char * bestguess;
    int len;
}comp_t;

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
void resettrie(node_t * );
node_t * createTrie();
int stringtoint(char []);
int commdetect(char []);
void printstring(char []);
void stringcopy(char [], char[]);
int verifyattempt (char [], char [], comp_t);
void resethistory(comp_t);
void printnumber(int);
int filterwords(node_t * , comp_t, int );
int checkcompatiblity(comp_t, char []);
int filterwords(node_t *, comp_t, int );
void filter(node_t *, int, int, char[], int *, comp_t);

int main(int argc, char * argv[]){

    char * struseless  __attribute__((unused));
    int8_t useless __attribute__((unused));

    int i;
    int command;
    int len;
    int inputmaxlen;
    int attempts;
    int res;
    int numwords;
    int8_t state;
    int8_t laststate; 
    comp_t progress;
    char tmp[LENMAX+1]; 
    node_t * root;

    struseless = fgets(tmp, LENMAX,stdin);
    len = stringtoint(tmp); 
    progress.len = len;
    if(len > COMMANDMAXLEN)
        inputmaxlen = len;
    else
        inputmaxlen = COMMANDMAXLEN;
    char inputstr[inputmaxlen+1];


    char bestguess[len+1] ;//__attribute__((unused));
    progress.bestguess = bestguess;
    char correct[len+1];
    root = createTrie(); 
    state=ADDWORDS;
    while(1){
        struseless = fgets(inputstr, inputmaxlen, stdin);
        if(inputstr[0] == COMMAND){
            command = commdetect(inputstr); 
            if(command == NEWGAME){
                state = NEWGAME;
                resettrie(root);
                resethistory(progress);
            }   
            else if(command == PRINTREMAINING){
                printwords(root, len);
            }
            else if(command == ADDWORDS){
                laststate=state;
                state=ADDWORDS;
            }
            else{
                if(state == ENDADD)
                    state = laststate;
            } 
        }
        else{
            if(state == INGAME){
                if(findword(root, inputstr)){
                    res = verifyattempt(correct, inputstr, progress);
                    if(res==0){
                        attempts--;
                        numwords = filterwords(root, progress, len);
                        printnumber(numwords);
                        useless = putchar_unlocked('\n');
                        if(attempts == 0){
                            printstring(GAMEOVER);
                            state = KO;
                        }
                    }
                }
                else{
                    printstring(NOTFOUND);
                }             
            }
            else if(state == ADDWORDS || state == KO){
                addword(root, inputstr);
            }
            else{
                stringcopy(correct, inputstr);
                for(i=0; correct[i] != '\0'; i++){
                    progress.correctocc[toindex(correct[i])]++;
                }
                struseless = fgets(tmp, LENMAX,stdin);
                attempts = stringtoint(tmp);
                state = INGAME; 
            }
        }
    }
    return 0;
}
int stringtoint(char seq[]){
    int res=0;
    int i;
    for(i=0; seq[i] != '\0'; i++)
        res = (res*10) + (seq[i]-STARTNUM);


    return res;
}
int commdetect(char seq[]){ //WE MAKE THE ASSUMPTION THAT USER WILL ALWAYS INSERT A CORRECT INPUT COMMAND
    if(seq[1] == 'n')  //"+nuova_partita" NEW GAME
        return 0;
    if(seq[1]=='s') // "+stampa_filtrate" PRINT REMAINING WORD
        return 1;
    if(seq[11]=='i') //"+inserisci_inizio" ADD NEW WORDS
        return 2;
    return 3; //"+inserisci_fine" END OF ADDING NEW WORDS
}
void printstring(char word[]){
    int8_t useless __attribute__((unused));
    for(int i=0; word[i] != '\0'; i++)
        useless = putchar_unlocked(word[i]);
    useless = putchar_unlocked('\n');
    return;
}
void stringcopy(char dest[], char origin[]){
    int i;
    i=-1;
    do{
        i++;
        dest[i] = origin[i];
    }while(origin[i] != '\0');
    return;
}
int verifyattempt (char correct[], char guess[], comp_t progress){
    int8_t res;
    int i;
    int len = progress.len;
    char outstr[len+1];
    int localcounter[DIM];
    int mincounter[DIM];
    int8_t exact[DIM];
    int tmp;


    for(i=0; i < DIM; i++){
        localcounter[i]=progress.correctocc[i];
        mincounter[i]=0;
        exact[i]=0;
    }   
    res=1;
    for(i=0; i < len; i++){
        if(guess[i] == correct[i]){
            outstr[i]=RIGHT;
            progress.bestguess[i]=correct[i];
        }
        else
            res=0;  
    }
    if(res == 1){
        printstring(WIN);
        return 1;
    }
    outstr[len] = '\0';
    for(i=0; i < len; i++){
        tmp = toindex(guess[i]);
        localcounter[tmp]--;
        if(outstr[i] != RIGHT){
            if(localcounter[tmp] >= 0){
                outstr[i]=WRONGPOS;
                mincounter[tmp]++;
            }
            else{
                outstr[i]=WRONG;
                exact[tmp]=1;
            }
        }
        else{
            mincounter[tmp]++;
        }
    }
    for(i=0; i < DIM; i++){
        if(progress.definitive[i] == 0){
            if(mincounter[i] > progress.counters[i])
                progress.counters[i]=mincounter[i];
            progress.definitive[i] = exact[i];
        }
    }
    printstring(outstr);
    return 0;

}
void resethistory(comp_t progress){
    int i;

    for(i=0; i < DIM; i++){
        progress.counters[i]=0;
        progress.definitive[i]=0;
        progress.correctocc[i]=0;
    }
    for(i=0; progress.bestguess[i] != '\0'; i++)
        progress.bestguess[i] = NOTHING;
    
    return;
}
void printnumber(int value){
    int8_t useless __attribute__((unused));
    int remaining = value/10;
    if(remaining !=0){
        printnumber(remaining);
    }
    useless = putchar_unlocked(value%10 + STARTNUM);
    return;
}
int checkcompatiblity(comp_t progress, char guess[]){
    int temp[DIM];
    int i;
    for(i=0; i < DIM; i++)
        temp[i]=0;
    for(i=0; guess[i] != '\0'; i++){
        if((*(progress.bestguess+i) != NOTHING) && (guess[i] != *((progress.bestguess)+i)))
            return 0;
        temp[toindex(guess[i])]++;     
    }
    for(i=0; i < DIM; i++){
        if((temp[i] < progress.counters[i]) || (temp[i] > progress.counters[i] && progress.definitive[i]==1))
            return 0;      
    }
    return 1;
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
void printwords(node_t * head, int dimmax){
    char wordtemp[dimmax+1];
    printer(head, 0, dimmax, wordtemp);
    return;  
}
void printer(node_t * elem, int pos, int max, char word[]){
    if(elem->state == EOW){
        word[pos]='\0';
        printstring(word);
    }
    for(int i=0; i < DIM; i++){
        if(elem->sons[i] != NULL && pos <= max){
            word[pos]= toletter(i);
            printer(elem->sons[i], pos+1, max, word);
        }
    }
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
int filterwords(node_t * head, comp_t progress, int dimmax){
    int res;
    char wordtemp[dimmax+1];
    res=0;
    filter(head, 0, dimmax, wordtemp, &res, progress);
    return res; 
}
void filter(node_t * elem, int pos, int max, char word[], int * countwords, comp_t progress){
    int res;
    if(elem->state == EOW){
        word[pos]='\0';
        res = checkcompatiblity(progress, word);
        if(res ==1)
            *countwords = *countwords+1;
    }
    for(int i=0; i < DIM; i++){
        if(elem->sons[i] != NULL && pos <= max){
            word[pos]= toletter(i);
            printer(elem->sons[i], pos+1, max, word);
        }
    }
    return;
}



