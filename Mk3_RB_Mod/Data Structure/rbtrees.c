#include <stdio.h>
#include <stdlib.h>

#define RED 0
#define BLACK 1
#define COMPATIBLE 1
#define INCOMPATIBLE 0
#define FOUND 1
#define NOTFOUND 0

//FOR THIS PROJECT PURPOSE METHODS FOR THE DELETION OF SPECIFIC NODES ARE NOT NEEDED 


typedef struct node_s{
    
    unsigned long long val;
    int8_t color;
    struct node_s * left;
    struct node_s * right;
    struct node_s * p;
    int8_t valid;
}node_t;


node_t * insert(node_t *, unsigned long long);
node_t * insertfixup (node_t *, node_t *);
node_t * leftrotate(node_t * , node_t * );
node_t * rightrotate(node_t * , node_t * );
int find(node_t *, unsigned long long);
void resettree(node_t *);
void printtree(node_t *);
void deletetree(node_t *);


node_t * insert(node_t * root, unsigned long long toins){
    node_t * new, * tmp, * father;

    do{
    new = malloc(sizeof(node_t)); 
    }while(new == NULL);

    father = root;
    tmp = root;

    while(tmp != NULL){
        father = tmp;
        if(tmp->val < toins){
            tmp = tmp->right;
        }
        else{
            tmp = tmp->left;
        }
    }
    new->val=toins;
    new->left = NULL;
    new->right = NULL;
    new->p = father;
    new->valid = COMPATIBLE;
    new->color = RED; 
    if(root == NULL){
        return new; 
    }
    if( father->val <= toins)
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
void printtree(node_t * root){
    if(root){
        printtree(root->left);
        printf("%llu ", root->val);
        printtree(root->right);
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
int find(node_t * root, unsigned long long tofind){
    while(root != NULL){
        if(root->val == tofind)
            return FOUND;
        if(root->val < tofind)
            root = root->right;
        else
            root = root->left;
    }
    return NOTFOUND;
}
void deletetree(node_t * root){
    if(root){
        deletetree(root->left);
        deletetree(root->right);
        free(root);
    }
    return;
}