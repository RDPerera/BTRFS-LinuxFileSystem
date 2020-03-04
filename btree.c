#include <stdio.h>
#include <stdlib.h>

#define M 4 //degree of b-tree

#define VAP 1 //Value already present
#define Available 2 
#define Success 3
#define LessKeys 4
#define NotFound 5


struct node {
        int n; // # keys in node
        int keys[M - 1]; // keys
        struct node *links[M]; //pointers to child nodes
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



void Insert(int key) {
        struct node *newnode;
        int upKey;
        int state;
        state = ins(root, key, &upKey, &newnode);//checking all cases to insert a new key
        if (state == VAP)
                printf("Key Already Present In B-Tree\n");
        if (state == Available) {
                struct node *uproot = root;
                root = (struct node*)malloc(sizeof(struct node));
                root->n = 1;
                root->keys[0] = upKey;
                root->links[0] = uproot;
                root->links[1] = newnode;
        }
}

int ins(struct node *link, int key, int *upKey, struct node **newnode) {
        struct node *newlink, *lastlink;
        int position, i, n, splitPosition;
        int newKey, lastKey;
        int state;
        if (link == NULL) {//if root is null
                *newnode = NULL;
                *upKey = key;
                return Available ;
        }
        n = link->n;
        position = searchPosition(key, link->keys, n);
        if (position < n && key == link->keys[position])
                return VAP;
        state = ins(link->links[position], key, &newKey, &newlink);
        if (state != Available )
                return state;
        //
        if (n < M - 1) {
                position = searchPosition(newKey, link->keys, n);
                /*Shifting the key and pointer right for Inserting the new key*/
                for (i = n; i>position; i--) {
                        link->keys[i] = link->keys[i - 1];
                        link->links[i + 1] = link->links[i];
                }
                /*Key is Inserted at exact location*/
                link->keys[position] = newKey;
                link->links[position + 1] = newlink;
                ++link->n; /*incrementing the number of keys in node*/
                return Success;
        }/*End of if */
        /*If keys in nodes are maximum and position of node to be Inserted is last*/
        if (position == M - 1) {
                lastKey = newKey;
                lastlink = newlink;
        }
        else {/*If keys in node are maximum and positionition of node to be Inserted is not last*/
                lastKey = link->keys[M - 2];
                lastlink = link->links[M - 1];
                for (i = M - 2; i>position; i--) {
                        link->keys[i] = link->keys[i - 1];
                        link->links[i + 1] = link->links[i];
                }
                link->keys[position] = newKey;
                link->links[position + 1] = newlink;
        }
        splitPosition = (M - 1) / 2;
        (*upKey) = link->keys[splitPosition];

        (*newnode) = (struct node*)malloc(sizeof(struct node));/*Right node after split*/
        link->n = splitPosition; /*No. of keys for left splitted node*/
        (*newnode)->n = M - 1 - splitPosition;/*No. of keys for right splitted node*/
        for (i = 0; i < (*newnode)->n; i++) {
                (*newnode)->links[i] = link->links[i + splitPosition + 1];
                if (i < (*newnode)->n - 1)
                        (*newnode)->keys[i] = link->keys[i + splitPosition + 1];
                else
                        (*newnode)->keys[i] = lastKey;
        }
        (*newnode)->links[(*newnode)->n] = lastlink;
        return Available;
}/*End of ins()*/

void display(struct node *link, int blanks) {
        if (link) {
                int i;
                for (i = 1; i <= blanks; i++)
                        printf(" ");
                for (i = 0; i < link->n; i++)
                        printf("%d ", link->keys[i]);
                printf("\n");
                for (i = 0; i <= link->n; i++)
                        display(link->links[i], blanks + 10);
        }/*End of if*/
}/*End of display()*/

void search(int key) {
        int position, i, n;
        struct node *link = root;
        printf("Search path:\n");
        while (link) {
                n = link->n;
                for (i = 0; i < link->n; i++)
                        printf(" %d", link->keys[i]);
                printf("\n");
                position = searchPosition(key, link->keys, n);
                if (position < n && key == link->keys[position]) {
                        printf("Key %d found in positionition %d of last dispalyed node\n", key, i);
                        return;
                }
                link = link->links[position];
        }
        printf("Key %d is not available\n", key);
}/*End of search()*/

int searchPosition(int key, int *keyList, int n) {
        int position = 0;
        while (position < n && key > keyList[position])
                position++;
        return position;
}/*End of searchPosition()*/

void Delete(int key) {
        struct node *uproot;
        int state;
        state = del(root, key);
        switch (state) {
        case 5:
                printf("Key %d is not available\n", key);
                break;
        case 4:
                uproot = root;
                root = root->links[0];
                free(uproot);
                break;
        default:
                return;
        }/*End of switch*/
}/*End of Delete()*/

int del(struct node *link, int key) {
        int position, i, pivot, n, min;
        int *keyList;
        int state;
        struct node **links, *leftLink, *rightLink;

        if (link == NULL)
                return NotFound;
        /*Assigns states of node*/
        n = link->n;
        keyList = link->keys;
        links = link->links;
        min = (M - 1) / 2;/*Minimum number of keys*/

                                          //Search for key to delete
        position = searchPosition(key, keyList, n);
        // links is a leaf
        if (links[0] == NULL) {
                if (position == n || key < keyList[position])
                        return NotFound;
                /*Shift keys and pointers left*/
                for (i = position + 1; i < n; i++)
                {
                        keyList[i - 1] = keyList[i];
                        links[i] = links[i + 1];
                }
                return --link->n >= (link == root ? 1 : min) ? 3 : 4;
        }/*End of if */

         //if found key but links is not a leaf
        if (position < n && key == keyList[position]) {
                struct node *tmpPtr = links[position], *tmpPtr1;
                int nkey;
                while (1) {
                        nkey = tmpPtr->n;
                        tmpPtr1 = tmpPtr->links[nkey];
                        if (tmpPtr1 == NULL)
                                break;
                        tmpPtr = tmpPtr1;
                }/*End of while*/
                keyList[position] = tmpPtr->keys[nkey - 1];
                tmpPtr->keys[nkey - 1] = key;
        }/*End of if */
        state = del(links[position], key);
        if (state != LessKeys)
                return state;

        if (position > 0 && links[position - 1]->n > min) {
                pivot = position - 1; /*pivot for left and right node*/
                leftLink = links[pivot];
                rightLink = links[position];
                /*Assigns states for right node*/
                rightLink->links[rightLink->n + 1] = rightLink->links[rightLink->n];
                for (i = rightLink->n; i>0; i--) {
                        rightLink->keys[i] = rightLink->keys[i - 1];
                        rightLink->links[i] = rightLink->links[i - 1];
                }
                rightLink->n++;
                rightLink->keys[0] = keyList[pivot];
                rightLink->links[0] = leftLink->links[leftLink->n];
                keyList[pivot] = leftLink->keys[--leftLink->n];
                return Success;
        }/*End of if */
         //if (positionn > min)
        if (position < n && links[position + 1]->n > min) {
                pivot = position; /*pivot for left and right node*/
                leftLink = links[pivot];
                rightLink = links[pivot + 1];
                /*Assigns states for left node*/
                leftLink->keys[leftLink->n] = keyList[pivot];
                leftLink->links[leftLink->n + 1] = rightLink->links[0];
                keyList[pivot] = rightLink->keys[0];
                leftLink->n++;
                rightLink->n--;
                for (i = 0; i < rightLink->n; i++) {
                        rightLink->keys[i] = rightLink->keys[i + 1];
                        rightLink->links[i] = rightLink->links[i + 1];
                }/*End of for*/
                rightLink->links[rightLink->n] = rightLink->links[rightLink->n + 1];
                return VAP;
        }/*End of if */

        if (position == n)
                pivot = position - 1;
        else
                pivot = position;

        leftLink = links[pivot];
        rightLink = links[pivot + 1];
        /*merge right node with left node*/
        leftLink->keys[leftLink->n] = keyList[pivot];
        leftLink->links[leftLink->n + 1] = rightLink->links[0];
        for (i = 0; i < rightLink->n; i++) {
                leftLink->keys[leftLink->n + 1 + i] = rightLink->keys[i];
                leftLink->links[leftLink->n + 2 + i] = rightLink->links[i + 1];
        }
        leftLink->n = leftLink->n + rightLink->n + 1;
        free(rightLink); /*Remove right node*/
        for (i = position + 1; i < n; i++) {
                keyList[i - 1] = keyList[i];
                links[i] = links[i + 1];
        }
        int s;
        if(link == root)
        	s=1;
        else
        	s=min;
        if(--link->n >= s)
        	return Success;
        else
        	return LessKeys;
}/*End of del()*/

void eatline(void) {
        char c;
        while ((c = getchar()) != '\n');
}

/* Function to display each key in the tree in sorted order (in-order traversal)
@param struct node *link, the pointer to the node you are currently working with
*/

void inorder(struct node *link) {
        if (link) {
                if (link->n >= 1) {
                        inorder(link->links[0]);
                        printf("%d ", link->keys[0]);
                        inorder(link->links[1]);
                        if (link->n == 2) {
                                printf("%d ", link->keys[1]);
                                inorder(link->links[2]);
                        }
                }
        }
}

/* Function that returns the total number of keys in the tree.
@param struct node *link, the pointer to the node you are currently working with
*/
int totalKeys(struct node *link) {
        if (link) {
                int count = 1;
                if (link->n >= 1) {
                        count += totalKeys(link->links[0]);
                        count += totalKeys(link->links[1]);
                        if (link->n == 2) count += totalKeys(link->links[2]) + 1;
                }
                return count;
        }
        return 0;
}

/* Function that prints the total number of keys in the tree.
@param struct node *link, the pointer to the node you are currently working with
*/
void printTotal(struct node *link) {
        printf("%d\n", totalKeys(link));
}

/* Function that returns the smallest key found in the tree.
@param struct node *link, the pointer to the node you are currently working with
*/
int getMin(struct node *link) {
        if (link) {
                int min;
                if (link->links[0] != NULL) min = getMin(link->links[0]);
                else min = link->keys[0];
                return min;
        }
        return 0;
}

/* Function that returns the largest key found in the tree.
@param struct node *link, the pointer to the node you are currently working with
*/
int getMax(struct node *link) {
        if (link) {
                int max;
                if (link->n == 1) {
                        if (link->links[1] != NULL) max = getMax(link->links[1]);
                        else max = link->keys[0];
                }
                if (link->n == 2) {
                        if (link->links[2] != NULL) max = getMax(link->links[2]);
                        else max = link->keys[1];
                }
                return max;
        }
        return 0;
}

/* Function that prints the smallest and largest keys found in the tree.
@param struct node *link, the pointer to the node you are currently working with
*/
void getMinMax(struct node *link) {
        printf("%d %d\n", getMin(link), getMax(link));
}

/* Function that determines the largest number.
@param int, integer to compare.
@param int, integer to compare.
@param int, integer to compare.
*/
int max(int first, int second, int third) {
        int max = first;
        if (second > max) max = second;
        if (third > max) max = third;
        return max;
}

/*Function that finds the maximum level in the node and returns it as an integer.
@param struct node *link, the node to find the maximum level for.
*/
int maxLevel(struct node *link) {
        if (link) {
                int l = 0, mr = 0, r = 0, max_depth;
                if (link->links[0] != NULL) l = maxLevel(link->links[0]);
                if (link->links[1] != NULL) mr = maxLevel(link->links[1]);
                if (link->n == 2) {
                        if (link->links[2] != NULL) r = maxLevel(link->links[2]);
                }
                max_depth = max(l, mr, r) + 1;
                return max_depth;
        }
        return 0;
}

/*Function that prints the maximum level in the tree.
@param struct node *link, the tree to find the maximum level for.
*/
void printMaxLevel(struct node *link) {
        int max = maxLevel(link) - 1;
        if (max == -1) printf("tree is empty\n");
        else printf("%d\n", max);
}
