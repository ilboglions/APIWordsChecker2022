#include <stdlib.h>
#include <stdio.h>
#define DIM 64 // ADMISSIBLE LETTERS a-z, A-Z, -,_ TOTAL 64 CHARS
#define NOTHING '#'

typedef struct compatibility_s{
    int counters[DIM];
    int8_t definitive[DIM];
    char * bestguess;
}comp_t;

int checkcompatiblity(comp_t, char []);



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


