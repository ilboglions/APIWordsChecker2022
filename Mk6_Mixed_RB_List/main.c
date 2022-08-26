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


typedef struct compatibility_s{ //STRUCTURE TO KEEP THE PROGRESS OF THE GAME, ALL THAT USER LEARNED FROM PAST ATTEMPTS 
    int counters[DIM]; //MINIMUM /CORRECT OCCURENCES OF THE CHARS LEARNED UNTIL NOW
    int8_t definitive[DIM]; // HAS THE USER LEARNED THE EXACT NUMBER OF OCCURENCES OF THAT CHAR? 1 IF YES 0 IF NO
    int correctocc[DIM]; //OCCURENCES OF THE CORRECT WORD, KEEPING IN MEMORY MAKES THE CHECK EASIER
    char * bestguess; //PARTIAL STRING LEARNED UNTIL NOW, IN EVERY POSITION '#' (NOTHING) IF USER DON'T KNOW STILL THE LETTER
    char * correct; //CORRECT STRING
    int len; // LENGTH OF STRINGS
    unsigned long long * wrongposarr; //ARRAY OF 64BIT INTS, IN EVERY POSITION OF THE ARRAY THE nth BIT INDICATES IF THE nth CHAR OF OUR ALPHABET SHOULDN'T 
                                      // BE FOUND IN THAT POSITION.
}comp_t;

typedef struct node_s{      //RB NODE 
    int8_t color; 
    struct node_s * left; //POINTER TO LEFT SON, NULL IF NOT EXISTS
    struct node_s * right; //POINTER TO RIGHT SON
    struct node_s * p; // POINTER TO FATHER, NULL IF THE NODE IS ALSO THE ROOT OF OUR TREE
    char word[]; //WORD KEPT IN THE NODE
}node_t;

typedef struct elem_s{  //LIST ELEMENT
	struct elem_s * next; //POINTER TO NEXT ELEM, NULL IF NOT EXISTS
    char word[]; //WORD KEPT IN THIS ELEMENT
} elem_t;

comp_t progress; // PROGRESS CREATED AS GLOBAL VARIABLE, EVERY FUNCTION CAN SEE THE PROGRESS.  
elem_t * stillvalids = NULL; //SAME AS ABOVE
//THE USING OF GLOBAL VARIABLES IS NOT A PROBLEM AT ALL SINCE THIS PROGRAM IS SINGLE-THREAD.


//PROTOTYPES


//RB TREES FUNCTIONS
node_t * insert(node_t *, char * , int);  //INSERT WORD INSIDE RB-TREE
node_t * insertfixup (node_t *, node_t *); //BALANCING THE RB-TREE AFTER INSERTION
node_t * leftrotate(node_t * , node_t * ); //ROTATING TWO NODES (SEE RB DOCUMENTATION)
node_t * rightrotate(node_t * , node_t * ); //ROTATING TWO NODES 
int find(node_t *, char * ); //FIND A WORD INSIDE RB-TREE
void deletetree(node_t *); //RB TREE FULL DELETION
node_t * deleteonrb(node_t * , node_t * ); //RB-TREE NODE DELETION, IN THIS VERSION WE DON'T REBALANCE THE TREE AFTER DELETION, FEATURE NOT NEEDED FOR OUR PURPOSE
void printtree(node_t *); //PRINT OF EVERY WORD IN THE TREE
node_t * min(node_t * ); //FINDS THE MINIMUM ELEMENT (FIRST IN LEXICOGRAPHIC ORDER) IN A SUBTREE
node_t * insertboth(node_t *  ,char [], int ); //INSERTION OF A WORD IN BOTH RB-TREE AND LIST (USEFUL WHEN WE INSERT NEW WORDS DURING THE GAME)


//LIST FUNCTIONS
elem_t * push (elem_t * , char[]); //INSERTS NEW ELEMENT AS FIRST OF THE LIST PUSHING FORWARD THE OLD ONES
elem_t * emptylist(elem_t * ); //DELETES THE LIST
void deleteelem(elem_t * ); //DELETES THE SON OF THE NODE GIVEN
elem_t * pop(elem_t *); //DELETES THE FIRST ELEMENT OF THE LIST
void printlist(elem_t * ); //PRINTS EVERY WORD IN THE LIST
int listlength(elem_t * ); //RETURNS THE NUMBER OF ELEMENTS IN THE LIST
elem_t * listmergesort(elem_t * ); //SORTS IN LEXICOGRAPHIC ORDER THE ELEMENTS OF THE LIST WITH MERGE SORT
elem_t * merge(elem_t * , elem_t * ); //MERGE IN ORDER TWO LISTS EACH ONE ALREADY SORTED


//CONVERTION FUNCTIONS
int stringtoint(char []); //CONVERTS STRING TO INT
int strcomp(char [], char[]); //COMPARES TWO STRINGS, 0 IF DIFFERENT AND THE FIRST IS BEFORE IN LEXICOGRAPHIC ORDER, 1 IF THE STRINGS ARE IDENTICAL, 2 OPPOSITE 
                            //SCENARIO THAN 0
void stringcopy(char [], char[]); //DUPLICATES A STRING
int toindex(char); //GIVES BACK THE INDEX OF A CHAR IN THE GAME ALPHABETH, LEGEND IN THE FUNCTION IMPLEMENTATION
char toletter(int); //RETURNS THE CHAR ASSOCIATED TO A INDEX


// I-O FUNCTIONS
int readstr(char [], int); //READ A STRING FROM STDIN OF MAX DIM CHARS. RETURNS 1 IF EVERYTHING IS FINE 0 IF BUFFER IS EMPTY
void printstring(char []); //PRINT A STRING
void printnumber(int); //PRINT A NUMBER IN STDOUT


//GAME FUNCTIONS
int commdetect(char []); //PARSING A COMMAND
int verifyattempt (char [], char [], char []); //VERIFICATION OF AN ATTEMPT, RETURNS 1 IF USER WIN 0 ELSE 
void resethistory(); //RESET OF THE PROGRESS, NEW GAME
int checkcompatibility(char []); //CHECK OF A WORD WITH ALL THE LEARNED CONSTRAINTS
int filterwords(elem_t *, char []); //ISPECTION OF THE LIST AND DELETION OF NOT COMPATIBLE WORD GIVEN THE CURRENT WRONG ATTEMPT (PARTIAL)
void firstfilterwords(node_t *, int *, char []); //FIRST SORTING OF WORDS, THIS FUNCTION POPULATES THE GAME LIST AT THE FIRST USER ATTEMPT WITH THE WORDS THAT 
                                                //ARE STILL COMPATIBLE.
int iscompatiblenow(char [], char []);          //LOCAL CHECK OF A WORD WITH PARTIAL REQUIREMENTS (BASED ON THE ACTUAL ATTEMPT, NOT ON ALL OF THEM)

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    int8_t firstattempt;
    int8_t state;
    int8_t laststate; 
    char tmp[LENMAX+1]; 
    node_t * root = NULL;


    readstr(tmp, LENMAX);  
    len = stringtoint(tmp); 
    progress.len = len;        //THE FIRST THING TO KNOW FROM USER IS THE LENGTH OF THE WORDS
    if(len > COMMANDMAXLEN)
        inputmaxlen = len+1;
    else
        inputmaxlen = COMMANDMAXLEN+1;
    char inputstr[inputmaxlen];  //INPUTMAXLEN AS EASY TO UNDERSTAND IS THE MAXIMUM DIMENSION OF AN INPUT STRING 
    unsigned long long wrongposlet[len]; //THE ARRAY OF 64BIT NUMBERS EXPLAINED AT LINE 43
    firstattempt=1; //FLAG, 1 IF USER HAS NOT PLAYED THE FIRST ATTEMPT OF THE GAME YET
    char outstr[len+1]; //OUPUT STRING FOR EVERY ATTEMPT
    char bestguess[len+1] ; 
    progress.wrongposarr = wrongposlet;
    progress.bestguess = bestguess;
    char correct[len+1];




    state=ADDWORDS; //INITIAL STATE IS ADDWORDS, UNTIL THE FIRST COMMAND THE USER IS GIVING US NEW WORDS TO KEEP ON THE TREE
    laststate = ADDWORDS; //ALSO LASTSTATE IS ADDWORDS, USEFUL IF THE FIRST COMMAND GIVEN BY USER IS "+inserisci_inizio", NOT A SMART MOVE BUT
                        // WE NEVER KNOW WHAT USERS COULD DO ;)
    while(readstr(inputstr, inputmaxlen)){ //GAMES LOOP, READING A NEW LINE FROM STDIN, IF BUFFER IS EMPTY THE PROGRAM MUST END
        if(inputstr[0] == COMMAND){ //IF USER IS GIVING US A COMMAND WHE NEED TO PARSE IT
            command = commdetect(inputstr); //PARSING THE COMMAND
            if(command == NEWGAME){ //THE PLAYER WANTS TO PLAY A NEW GAME, CHANGE THE STATE OF THE GAME AND RESET PROGRESS STRUCTURE
                state = NEWGAME;
                resethistory();
            }   
            else if(command == PRINTREMAINING){ //THE PLAYER WANTS ALL THE REMAINING WORDS
                if(firstattempt){ //IF THE PLAYER HAS NOT PLAYED YET WE PRINT ALL THE TREE WORDS
                    printtree(root); 
                }
                else{ //ELSE WHE SORT THE LIST OF THE STILL COMPATIBLE WORD AND WE PRINT IT
                    stillvalids = listmergesort(stillvalids);
                    printlist(stillvalids);
                }
            }
            else if(command == ADDWORDS){ //THE PLAYER WANTS TO ADD NEW WORDS TO THE TREE/LIST
                laststate=state; //THIS GIVES THE OPPORTUNITY TO COME BACK TO THE CURRENT STATE AFTER ADDING NEW WORDS
                state=ADDWORDS;
            }
            else{
                if(state == ADDWORDS) 
                    state = laststate; //COMING BACK TO THE LAST STATE BEFORE ADDING WORDS 
            } 
        }
        else{ //USER IS NOT GIVING A COMMAND
            if(state == INGAME){ //IF THE STATE IS INGAME THE STRING RECEIVED IS AN ATTEMPT
                if(find(root, inputstr)){ //WE NEED FIRST OF ALL TO FIND THE WORD IN THE TREE, IF IT'S NOT PRESENT WE DON'T EVEN CONSIDER IT
                    res = verifyattempt(correct, inputstr, outstr); //WE VERIFY THIS ATTEMPT
                    if(res==0){ //IF THE ATTEMPT IS NOT CORRECT WE DECREMENT THE NUMBER OF AVAILABLE TRIES
                        attempts--;
                           
                        if(firstattempt){ //IF THIS IS THE FIRST ATTEMPT 
                            firstfilterwords(root, &numwords, inputstr); //WE CREATE THE LIST OF STILL COMPATIBLE WORDS
                            firstattempt=0; //CHANGE THE FLAG
                        }
                        else{
                            numwords = filterwords(stillvalids, inputstr); //WE FILTER THE WORDS ON THE LIST BASING THE PARSING ON THIS SPECIFIC ATTEMPT
                        }
                        printnumber(numwords); //WE PRINT THE NUMBER OF WORDS STILL COMPATIBLE
                        useless = putchar_unlocked('\n');
                        if(attempts == 0){ //IF THE PLAYER TRIED THE LAST ATTEMPT WE CLOSE THE GAME, STATE IS KO 
                            printstring(GAMEOVER);
                            state = KO;
                            firstattempt=1;
                        }
                    }
                    else{ //IF THE PLAYER GUESSED THE CORRECT WORD WE COME BACK TO ADDWORDS STATE
                        state = ADDWORDS;
                    }
                }
                else{ //IF THE ATTEMPT WAS NOT VALID (WORD NOT PRESENT IN THE TREE)
                    printstring(NOTEXISTS);
                }             
            }
            else if(state == ADDWORDS || state == KO){ //WE HAVE TO ADD NEW WORDS
                if(firstattempt){ //WE ARE NOT IN A GAME OR USER HAS NOT PLAYED YET WE INSERT DIRECTLY OF THE TREE
                    root = insert(root, inputstr, len); 
                }
                else{ //WE ARE IN THE MIDDLE OF A GAME AND WE HAVE TO INSERT THE WORD IN THE TREE AND (MAYBE) ALSO IN THE LIST OF COMPATIBLE WORDS 
                    root = insertboth(root, inputstr, len);
                }

            }
            else{ //STATE IS NEWGAME
                stringcopy(correct, inputstr); //THE STRING GIVEN IS THE SOLUTION
                for(i=0; correct[i] != '\0'; i++){
                    progress.correctocc[toindex(correct[i])]++;
                }
                progress.correct = correct;
                readstr(tmp, LENMAX);
                attempts = stringtoint(tmp); //NUMBER OF MAX ATTEMPTS BEFORE GAME OVER
                state = INGAME; //STATE IS INGAME
                numwords=0;
                stillvalids = emptylist(stillvalids); //DELETION OF OLD GAME LIST
                firstattempt=1;
            }
        }
    }
    stillvalids = emptylist(stillvalids); //FINAL DELETION OF LIST AND TREE, THE PROGRAM IS ENDING ITS EXECUTION
    deletetree(root);
    return 0;
}

//RB TREES FUNCTIONS
node_t * insert(node_t * root, char * toins, int len){
    node_t * new, * tmp, * father;
    do{
    new = malloc(sizeof(node_t) + len +1); 
    }while(new == NULL); //CREATION OF THE NEW NODE

    father = root;
    tmp = root;

    while(tmp != NULL){ //FINDING THE PLACE TO INSERT THE NEW NODE
        father = tmp;
        if(!strcomp(tmp->word, toins)){
            tmp = tmp->right;
        }
        else{
            tmp = tmp->left;
        }
    }
    stringcopy(new->word, toins); //COPY THE STRING INSIDE THE NEW NODE
    new->left = NULL; 
    new->right = NULL;
    new->p = father; //LINK NEW NODE TO HIS FATHER
    new->color = RED; //SET NEW NODE COLOR TO RED 
    if(root == NULL){ //IF THE TREE IS EMPTY RETURN THE NEW NODE
        return new; 
    }
    if(!strcomp(father->word, toins)) //LINK FATHER TO NEW NODE (SON)
        father->right = new;
    else
        father->left = new;

    root = insertfixup(root, new); //REBALANCE THE TREE

    return root;
}
node_t * insertfixup(node_t * root, node_t * elem){ //VIEW DOCUMENTATION
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
int find(node_t * root, char tofind[]){
    int8_t res;
    while(root != NULL){
        res = strcomp(root->word, tofind);
        if (res==1)
            return FOUND;
        if(res==0)
            root = root->right;
        else
            root = root->left;
    }
    return NOTFOUND;
}
void deletetree(node_t * root){    
    if(root){
        deletetree(root->left); //FREE LEFT SUBTREE
        deletetree(root->right); //FREE RIGHT SUBTREE
        free(root); //FREE ACTUAL NODE
    }
    return;
}
node_t * deleteonrb(node_t * root, node_t * x){
    node_t * todel;
    if(root && x){
        if(x->left == NULL){
            if(x->p){
                if(x == x->p->right)
                    x->p->right = x->right;
                else
                    x->p->left = x->right;
            }
            else{
                root = x->right;
            }
            if(x->right)
                x->right->p = x->p;
            free(x);
        }
        else if(x->right == NULL){
            if(x->p){
                if(x == x->p->right)
                    x->p->right = x->left;
                else
                    x->p->left = x->left;
            }
            else{
                root = x->left;
            }
            if(x->left)
                x->left->p = x->p;
            free(x);
        }
        else{
            todel = min(x->right);
            stringcopy(x->word, todel->word);
            if(todel->p){
                if(todel == todel->p->right)
                   todel->p->right = todel->right;
                else
                    todel->p->left = todel->right;
            }
            if(todel->right){
                todel->right->p = todel->p;
            }
            free(todel);
        }
    }
    return root;
}
void printtree(node_t * root){
    if(root){
        printtree(root->left);
        printstring(root->word);
        printtree(root->right);
    }
    return;
}
node_t * min(node_t * elem){
    while(elem->left)
        elem=elem->left;
    return elem;
}
node_t * insertboth(node_t * root ,char toins[], int len){
    if(checkcompatibility(toins))
        stillvalids = push(stillvalids, toins);
    return insert(root, toins, len);
}

//LIST FUNCTIONS
elem_t * push (elem_t * head, char toins[]){
	elem_t * tmp = NULL;
    int len = progress.len;
    do{
        tmp = malloc(sizeof(elem_t) + len +1);
    }while(tmp == NULL);
	stringcopy(tmp->word, toins);
	tmp->next = head;
	return tmp;
}
elem_t * emptylist(elem_t * head){
	if(head){
		if(head->next != NULL){
			head->next = emptylist(head->next);
		}
		free(head);
	}
	return NULL;
}
void deleteelem(elem_t * father){
    elem_t * todel;
    todel = father->next;
    father->next = father->next->next;
    free(todel);
    return;
}
elem_t * pop (elem_t * head){
    elem_t * todel;
    todel = head;
    head = head->next;
    free(todel);
    return head;
}
void printlist(elem_t * head){
	while(head != NULL){
		printstring(head->word);
		head = head->next;
	}
}
int listlength(elem_t * head){
	int count=0;
	elem_t * tmp;
	tmp = head;
	while(tmp){
		count++;
		tmp = tmp->next;
	}
	return count;
}
elem_t * listmergesort(elem_t * head){
	int len;
	int i;
	int lim;
	elem_t * second;
	elem_t * tmp;

	len = listlength(head);
	if(len > 1){
		lim = (len/2)-1;
		tmp = head;
		for(i=0; i < lim; i++){
			tmp = tmp->next;
		}
		second = tmp->next;
		tmp ->next = NULL;
		head=listmergesort(head);
		second=listmergesort(second);

		head = merge (head, second);
	}
	return head;
}
elem_t * merge(elem_t * first, elem_t * second){
	elem_t * newhead;
	elem_t * last;
	if(!strcomp(first->word,second->word)){
		newhead= first;
		first = first->next;
	}
	else{
		newhead= second;
		second= second->next;
	}
	last=newhead;
	while(first != NULL && second != NULL){
		if(!strcomp(first->word,second->word)){
			last->next = first;
			first = first->next;
		}
		else{
			last->next = second;
			second = second->next;
		}
		last = last->next;
	}
	if(first != NULL){
		last->next = first;
	}
	else
		last->next = second;


	return newhead;
}

//CONVERSION FUNCTIONS
int stringtoint(char seq[]){
    int res=0;
    int i;
    for(i=0; seq[i] <= ENDNUM && seq[i] >= STARTNUM; i++)
        res = (res*10) + (seq[i]-STARTNUM);


    return res;
}
int strcomp(char first[], char second[]){
    int i;
    for(i=0; (first[i]==second[i]) && (first[i] != '\0'); i++){
        ;
    }
    if(first[i]<second[i])
        return 0;
    if(first[i]=='\0')
        return 1;
    return 2;
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

//I-O FUNCTIONS
int readstr(char str[], int dim){
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
    if(feof(stdin))
            return 0;
    return 1;
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
int verifyattempt (char correct[], char guess[], char outstr[]){
    int8_t res;
    int8_t i;
    int len = progress.len;
    int localcounter[DIM];
    int mincounter[DIM];
    unsigned long long notinthatpos;
    int8_t exact[DIM];
    int tmp;


    for(i=0; i < DIM; i++){  //RESET SITUATION
        localcounter[i]=progress.correctocc[i];
        mincounter[i]=0;
        exact[i]=0;
    }   
    res=1;
    for(i=0; i < len; i++){
        if(guess[i] == correct[i]){
            outstr[i]=RIGHT;
            progress.bestguess[i]=correct[i];
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
            progress.wrongposarr[i] = progress.wrongposarr[i] | notinthatpos;
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
        if(progress.definitive[i] == 0){
            if(mincounter[i] > progress.counters[i])
                progress.counters[i]=mincounter[i];
            progress.definitive[i] = exact[i];
        }
    }
    printstring(outstr);
    return 0;

}
void resethistory(){
    int i;

    for(i=0; i < DIM; i++){
        progress.counters[i]=0;
        progress.definitive[i]=0;
        progress.correctocc[i]=0;
    }
    for(i=0; i < progress.len; i++){
        progress.bestguess[i] = NOTHING;
        progress.wrongposarr[i] = 0;
    }
    progress.bestguess[progress.len] = '\0';
    return;
}
int checkcompatibility(char guess[]){

    int temp[DIM];
    int index;
    int i;
    unsigned long long tmp;
    char * correct = progress.bestguess;
    unsigned long long * wrongposarr = progress.wrongposarr;
    int * counters = progress.counters;
    int8_t * definitive = progress.definitive;
    for(i=0; i < DIM; i++)
        temp[i]=0;
    for(i=0;  guess[i] != '\0'; i++){
        index =toindex(guess[i]);
        if((correct[i] != NOTHING) && (guess[i] != correct[i])){
            return INCOMPATIBLE;
        }
        temp[index]++; 
        tmp = wrongposarr[i] >> index;
        if(tmp & 1){
            return INCOMPATIBLE;  
        }   
    }
    
    for(i=0; i < DIM; i++){
        if((temp[i] < counters[i]) || (temp[i] != counters[i] && definitive[i]==1)){
            return INCOMPATIBLE; 
        }     
    }
    return COMPATIBLE;
}
int filterwords(elem_t * head, char wrongword[]){
    int res = 0;
    int tmp;
    elem_t * swap;

    if(head){
        tmp = iscompatiblenow(head->word, wrongword);
        while(head && (tmp == INCOMPATIBLE)){
            head = pop(head);
            tmp = iscompatiblenow(head->word, wrongword);
        }
        stillvalids = head;
        if(head){
            res++;
            while(head && head->next){
                tmp = iscompatiblenow(head->next->word, wrongword);
                res += tmp;
                swap = head;
                if(tmp == INCOMPATIBLE){
                    deleteelem(swap);
                }
                else
                    head = head->next;
            }
        } 
    } 
    return res;
}
void firstfilterwords(node_t * root, int * numwords, char wrongword[]){
    int res;
    if(root){
        res = iscompatiblenow(root->word, wrongword);
        * numwords = (*numwords) + res;
        if(res){
            stillvalids = push(stillvalids, root->word);
        }
        firstfilterwords(root->left, numwords,wrongword);
        firstfilterwords(root->right, numwords, wrongword);
    }
    return;
}
int iscompatiblenow(char guess[], char wrongword[]){
        int i;
        int temp[DIM];
        char * bestguess = progress.bestguess;
        char * correct = progress.correct;
        int * counters = progress.counters;
        int8_t * definitive = progress.definitive;
        int len = progress.len;
        int index;

        for(i=0; i < len; i++){
            index = toindex(guess[i]);
            temp[index]=0;
            if(bestguess[i] != NOTHING){
                index = toindex(correct[i]);
                temp[index]=0;
            }
            index = toindex(wrongword[i]);
            temp[index]=0;
            if((guess[i] == wrongword[i]) && (wrongword[i] != correct[i])){
                return INCOMPATIBLE;
            }
        }
        for(i=0;  i < len; i++){
            index =toindex(guess[i]);
            if((bestguess[i] != NOTHING) && (guess[i] != correct[i])){
                return INCOMPATIBLE;
            }
            temp[index]++;  
        }
        for(i=0;  i < len; i++){
           index = toindex(guess[i]);
           if((temp[index] < counters[index]) || (temp[index] != counters[index] && definitive[index]==1)){
                return INCOMPATIBLE;
           }
           if(bestguess[i] != NOTHING){
            index = toindex(correct[i]);
            if((temp[index] < counters[index]) || (temp[index] != counters[index] && definitive[index]==1)) 
                    return INCOMPATIBLE; 
           }
           index = toindex(wrongword[i]);
           if((temp[index] < counters[index]) || (temp[index] != counters[index] && definitive[index]==1)) 
                return INCOMPATIBLE;
        }
        return COMPATIBLE;
}



