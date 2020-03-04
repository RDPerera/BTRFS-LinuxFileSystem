#include <stdio.h>
#include <stdlib.h>
#include "btree.c"
#define M 4

#define VAP 1 //Value already present
#define Available 2 
#define Success 3
#define LessKeys 4
#define NotFound 5


struct node {
        int n; /* n < M No. of keys in node will always less than order of B tree */
        int keys[M - 1]; /*array of keys*/
        struct node *links[M]; /* (n+1 pointers will be in use) */
};
struct node * root = NULL;
//Duplicate 1 InsertIt2  Success3 SearchFailure5 lesskey4 

void Insert(int key);
void display(struct node *root, int);
void Delete(int x);
void search(int x);
int ins(struct node *r, int x, int* y, struct node** u);
int searchPosition(int x, int *keyList, int n);
int del(struct node *r, int x);
void eatline(void);
void inorder(struct node *link);
int totalKeys(struct node *link);
void printTotal(struct node *link);
int getMin(struct node *link);
int getMax(struct node *link);
void getMinMax(struct node *link);
int max(int first, int second, int third);
int maxLevel(struct node *link);
void printMaxLevel(struct node *link);


