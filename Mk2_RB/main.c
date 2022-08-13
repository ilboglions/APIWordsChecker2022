#include <stdio.h>
#include <stdlib.h>

#define RED 0
#define BLACK 1
#define COMPATIBLE 1
#define INCOMPATIBLE 0
#define FOUND 1
#define NOTFOUND 0
#define NOTHING '$'
#define DIM 64 // ADMISSIBLE LETTERS a-z, A-Z, -,_ TOTAL 64 CHARS
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
#define NOTEXISTS "not_exists\0"
#define KO -1
#define GAMEOVER "ko\0"
#define WIN "ok\0"
#define RIGHT '+'
#define WRONG '/'
#define WRONGPOS '|'
#define LASTCIPHER 63


typedef struct compatibility_s{
    int counters[DIM];
    int8_t definitive[DIM];
    int correctocc[DIM];
    char * bestguess;
    unsigned long long * wrongposarr;
    int len;
}comp_t;
typedef struct node_s{
    
    unsigned long long val;
    int8_t color;
    struct node_s * left;
    struct node_s * right;
    struct node_s * p;
    int8_t valid;
}node_t;


//PROTOTYPES


//RB TREES FUNCTIONS
node_t * insert(node_t *, unsigned long long *);
node_t * insertfixup (node_t *, node_t *);
node_t * leftrotate(node_t * , node_t * );
node_t * rightrotate(node_t * , node_t * );
int find(node_t *, unsigned long long *);
void printtree(node_t *, char [], int );
void resettree(node_t *);
void deletetree(node_t *);


//CONVERTION FUNCTIONS
int stringtoint(char []);
void stringcopy(char [], char[]);
int toindex(char);
char toletter(int);
void encodestring (char [], unsigned long long *);
void decodenumber(unsigned long long , char [], int );


// I-O FUNCTIONS
void readstr(char [], int);
void printstring(char []);
void printnumber(int);


//GAME FUNCTIONS
int commdetect(char []);
int verifyattempt (char [], char [], comp_t *, char []);
void resethistory(comp_t * );
int checkcompatiblity(comp_t * , char []);
int filterwords(node_t * , comp_t * , char []);


//MAIN FUNCTION//
int main(int argc, char * argv[]){

    int8_t useless __attribute__((unused));

    int i;
    int command;
    int len;
    int inputmaxlen;
    int attempts;
    int res;
    int numwords;
    unsigned long long encoded;
    int8_t state;
    int8_t laststate; 
    comp_t progress;
    char tmp[LENMAX+1]; 
    node_t * root = NULL;

    readstr(tmp, LENMAX);
    len = stringtoint(tmp); 
    progress.len = len;
    if(len > COMMANDMAXLEN)
        inputmaxlen = len+1;
    else
        inputmaxlen = COMMANDMAXLEN+1;
    char inputstr[inputmaxlen];
    unsigned long long wrongposlet[len];

    char outstr[len+1];
    char bestguess[len+1] ;//__attribute__((unused));
    progress.wrongposarr = wrongposlet;
    progress.bestguess = bestguess;
    char correct[len+1];
    state=ADDWORDS;
    laststate = ADDWORDS;
    while(1){
        readstr(inputstr, inputmaxlen);
        if(feof(stdin)){
            break;
        }
        if(inputstr[0] == COMMAND){
            command = commdetect(inputstr); 
            if(command == NEWGAME){
                state = NEWGAME;
                resettree(root);
                resethistory(&progress);
            }   
            else if(command == PRINTREMAINING){
                printtree(root, outstr, len);
            }
            else if(command == ADDWORDS){
                laststate=state;
                state=ADDWORDS;
            }
            else{
                if(state == ADDWORDS)
                    state = laststate;
                useless = filterwords(root, &progress, outstr);
            } 
        }
        else{
            if(state == INGAME){
                encodestring(inputstr, &encoded);
                if(find(root, &encoded)){
                    res = verifyattempt(correct, inputstr, &progress, outstr);
                    if(res==0){
                        attempts--;
                        numwords = filterwords(root, &progress, outstr);    
                        printnumber(numwords);
                        useless = putchar_unlocked('\n');
                        if(attempts == 0){
                            printstring(GAMEOVER);
                            state = KO;
                        }
                    }
                    else{
                        state = ADDWORDS;
                    }
                }
                else{
                    printstring(NOTEXISTS);
                }             
            }
            else if(state == ADDWORDS || state == KO){
                encodestring(inputstr, &encoded);
                root = insert(root, &encoded);
            }
            else{
                stringcopy(correct, inputstr);
                for(i=0; correct[i] != '\0'; i++){
                    progress.correctocc[toindex(correct[i])]++;
                }
                readstr(tmp, LENMAX);
                attempts = stringtoint(tmp);
                state = INGAME; 
            }
        }
    }
    deletetree(root);
    return 0;
}


//RB TREES FUNCTIONS
node_t * insert(node_t * root, unsigned long long * toins){
    node_t * new, * tmp, * father;

    do{
    new = malloc(sizeof(node_t)); 
    }while(new == NULL);

    father = root;
    tmp = root;

    while(tmp != NULL){
        father = tmp;
        if(tmp->val < * toins){
            tmp = tmp->right;
        }
        else{
            tmp = tmp->left;
        }
    }
    new->val= * toins;
    new->left = NULL;
    new->right = NULL;
    new->p = father;
    new->valid = COMPATIBLE;
    new->color = RED; 
    if(root == NULL){
        return new; 
    }
    if( father->val <= * toins)
        father->right = new;
    else
        father->left = new;
    root = insertfixup(root, new);
    return root;
}
node_t * insertfixup(node_t * root, node_t * elem){
    node_t * father;
    node_t * uncle;
    father = elem->p;

    while(father && father -> color == RED && father != root){
        if(father == father->p->left){ //FATHER IS A LEFT SON
            uncle = father->p->right;
            if(uncle && uncle->color == RED){ //CASE 1;
                father->color = BLACK;
                uncle->color = BLACK;
                father->p->color = RED;
                elem = father->p;
                father = elem->p;
            }
            else{
                if(elem == father->right){ //CASE 2
                    root = leftrotate(root, father);
                    elem = father;
                    father = elem->p;
                }
                father->color = BLACK;
                father->p->color = RED;
                root = rightrotate(root, father->p);
            }  
        }
        else{ //FATHER IS A RIGHT SON
            uncle = father->p->left;
            if(uncle && uncle->color == RED){ //CASE 1;
                father->color = BLACK;
                uncle->color = BLACK;
                father->p->color = RED;
                elem = father->p;
                father = elem->p;
            }
            else{
                if(elem == father->left){ //CASE 2
                    root = rightrotate(root, father);
                    elem = father;
                    father = elem->p;
                }
                father->color = BLACK;
                father->p->color = RED;
                root = leftrotate(root, father->p);
            } 
        }
    }

    return root;
}
node_t * leftrotate(node_t * root, node_t * elem){
    node_t * rightson;
    rightson = elem->right;
    elem->right = rightson->left;
    if(rightson->left != NULL){
        rightson->left->p = elem;
    }
    rightson->p = elem->p;
    if(elem == root){
        root = rightson;
    }
    else{
        if(elem == elem->p->left){
            elem->p->left = rightson;
        }
        else{
            elem->p->right = rightson;
        }
    }
    rightson -> left = elem;
    elem->p = rightson;
    return root;
}
node_t * rightrotate(node_t * root, node_t * elem){
        node_t * leftson;
    leftson = elem->left;
    elem->left = leftson->right;
    if(leftson->right != NULL){
        leftson->right->p = elem;
    }
    leftson->p = elem->p;
    if(elem == root){
        root = leftson;
    }
    else{
        if(elem == elem->p->left){
            elem->p->left = leftson;
        }
        else{
            elem->p->right = leftson;
        }
    }
    leftson -> right = elem;
    elem->p = leftson;
    return root;
}
int find(node_t * root, unsigned long long * tofind){
    while(root != NULL){
        if(root->val == *tofind)
            return FOUND;
        if(root->val < *tofind)
            root = root->right;
        else
            root = root->left;
    }
    return NOTFOUND;
}
void printtree(node_t * root, char str[], int len){
    if(root){
        printtree(root->left, str, len);
        if(root->valid == COMPATIBLE){
            decodenumber(root->val, str, len);
            printstring(str);
        }
        printtree(root->right, str, len);
    }
    return;
}
void resettree(node_t * root){
    if(root){
        resettree(root->left);
        root->valid = COMPATIBLE;
        resettree(root->right);
    }
    return;
}
void deletetree(node_t * root){    

    if(root){
        deletetree(root->left);
        deletetree(root->right);
        free(root);
    }
    return;
}


//CONVERSION FUNCTIONS
int stringtoint(char seq[]){
    int res=0;
    int i;
    for(i=0; seq[i] <= ENDNUM && seq[i] >= STARTNUM; i++)
        res = (res*10) + (seq[i]-STARTNUM);


    return res;
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
void encodestring (char str[], unsigned long long * out){

    int i;
    * out = toindex(str[0]);
    for(i=1; str[i] != '\0'; i++){
        * out = (* out << 6) + toindex(str[i]);
    }
    return;
}
void decodenumber(unsigned long long  out, char str[], int len){
    int i;
    for(i=len-1; i >=0; i--){
        str[i]= toletter(out & LASTCIPHER);
        out =  out >> 6;     
    }
    str[len]='\0';
    return;
}


//I-O FUNCTIONS
void readstr(char str[], int dim){
    int i;
    char tmp;
    i=0;
    tmp = getchar_unlocked();
    while(i < dim && tmp !='\n'){
        str[i] = tmp;
        i++;
        tmp = getchar_unlocked();
    }
    str[i] ='\0';
    return;
}
void printstring(char word[]){
    int8_t useless __attribute__((unused));
    for(int i=0; word[i] != '\0'; i++)
        useless = putchar_unlocked(word[i]);
    useless = putchar_unlocked('\n');
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


//GAME FUNCTIONS
int commdetect(char seq[]){ //WE MAKE THE ASSUMPTION THAT USER WILL ALWAYS INSERT A CORRECT INPUT COMMAND
    if(seq[1] == 'n')  //"+nuova_partita" NEW GAME
        return 0;
    if(seq[1]=='s') // "+stampa_filtrate" PRINT REMAINING WORD
        return 1;
    if(seq[11]=='i') //"+inserisci_inizio" ADD NEW WORDS
        return 2;
    return 3; //"+inserisci_fine" END OF ADDING NEW WORDS
}
int verifyattempt (char correct[], char guess[], comp_t * progress, char outstr[]){
    int8_t res;
    int8_t i;
    int len = progress->len;
    int localcounter[DIM];
    int mincounter[DIM];
    unsigned long long notinthatpos;
    int8_t exact[DIM];
    int tmp;


    for(i=0; i < DIM; i++){  //RESET SITUATION
        localcounter[i]=progress->correctocc[i];
        mincounter[i]=0;
        exact[i]=0;
    }   
    res=1;
    for(i=0; i < len; i++){
        if(guess[i] == correct[i]){
            outstr[i]=RIGHT;
            progress->bestguess[i]=correct[i];
            localcounter[toindex(guess[i])]--;
        }
        else{
            res=0; 
            outstr[i] = NOTHING;
        } 
    }
    if(res == 1){
        printstring(WIN);
        return 1;
    }
    outstr[len] = '\0';
    for(i=0; i < len; i++){
        tmp = toindex(guess[i]);
        if(outstr[i] != RIGHT){
            notinthatpos = 1;
            notinthatpos = notinthatpos << tmp;
            progress->wrongposarr[i] = progress->wrongposarr[i] | notinthatpos;
            localcounter[tmp]--;
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
        if(progress->definitive[i] == 0){
            if(mincounter[i] > progress->counters[i])
                progress->counters[i]=mincounter[i];
            progress->definitive[i] = exact[i];
        }
    }
    printstring(outstr);
    return 0;

}
void resethistory(comp_t * progress){
    int i;

    for(i=0; i < DIM; i++){
        progress->counters[i]=0;
        progress->definitive[i]=0;
        progress->correctocc[i]=0;
    }
    for(i=0; i < progress->len; i++){
        progress->bestguess[i] = NOTHING;
        progress->wrongposarr[i] = 0;
    }
    progress->bestguess[progress->len] = '\0';
    return;
}
int checkcompatiblity(comp_t * progress, char guess[]){

    int temp[DIM];
    int i;
    int index;
    unsigned long long tmp;
    for(i=0; i < DIM; i++)
        temp[i]=0;
    for(i=0;  guess[i] != '\0'; i++){
        index =toindex(guess[i]);
        if((progress->bestguess[i] != NOTHING) && (guess[i] != progress->bestguess[i])){
            return 0;
        }
        temp[index]++; 
        tmp = progress->wrongposarr[i] >> index;
        if(tmp & 1){
            return 0;  
        }   
    }
    
    for(i=0; i < DIM; i++){
        if((temp[i] < progress->counters[i]) || (temp[i] != progress->counters[i] && progress->definitive[i]==1)){
            return 0; 
        }     
    }
    return 1;
}
int filterwords(node_t * root, comp_t * progress, char str[]){
    int res = 0;
    if(root){
        if(root->valid == COMPATIBLE){
            decodenumber(root->val, str, progress->len);
            res = checkcompatiblity(progress, str);
            if(res == 0)
                root->valid = INCOMPATIBLE;
        }
        res = res + filterwords(root->left, progress, str) + filterwords(root->right, progress, str);
    }
    return res;
}



