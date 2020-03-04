#include<iostream>
using namespace std;



#define M 3 	//degree of b-tree
#define VAP 1 	//Value already present
#define Available 2 
#define Success 3
#define LessKeys 4
#define NotFound 5


/*************************   Disk 1   ******************************************/
char disk1[1000][1000];
/*************************   Disk 2   ******************************************/
char disk1[1000][1000];
/*******************************************************************************/
struct 
struct node {
        int n; 			 // # keys in node
        int keys[M - 1]; // keys
        node *links[M];	 //pointers to child nodes
};
node * root = NULL;


class btree{
	public:
		void Insert(int key) {
	        struct node *newnode;
	        int upKey;
	        int state;
	        state = ins(root, key, &upKey, &newnode);//checking all cases to insert a new key
	        if (state == VAP)
	                printf("Key Already Present In B-Tree\n");
	        if (state == Available) {
	                node *uproot = root;
	                root = new struct node;
	                root->links[1] = newnode;
	                root->keys[0] = upKey;
	                root->links[0] = uproot;
	                root->n = 1;
	                
	        	}
		}
		void Delete(int key) {
		        struct node *uproot;
		        int state;
		        state = del(root, key);
		        if(state==4)
		        {
		        	uproot = root;
		            root = root->links[0];
		            delete uproot;
				}
				else if(state==5)
				{
					printf("Key %d is not available\n", key);
				}
				else
				{
					return;
				}
		}
		
		
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
		}
		int ins(struct node *link, int key, int *upKey, struct node **newnode) {
		        node *newlink, *lastlink;
		        int position, i, n, splitPosition;
		        int newKey, lastKey;
		        int state;
		        if (link == NULL) { //if root is null
		        		*upKey = key;
		                *newnode = NULL;
		                
		                return Available ;
		        }
		        else{
			        n = link->n;
			        position = searchPosition(key, link->keys, n);
			        if (position < n && key == link->keys[position])
			                return VAP;
			        state = ins(link->links[position], key, &newKey, &newlink);
			        if (state != Available )
			                return state;
			        if (n < M - 1) {
			                position = searchPosition(newKey, link->keys, n);
			                i = n;
			                while(i>position)
			                {
			                	link->keys[i] = link->keys[i - 1];
			                    link->links[i + 1] = link->links[i];
			                    i--;
							}
			                link->keys[position] = newKey;
			                link->links[position + 1] = newlink;
			                ++link->n; 
			                return Success;
			        }
			        else{}
			        if (position == M - 1) {
			                lastKey = newKey;
			                lastlink = newlink;
			        }
			        else {
			                lastKey = link->keys[M - 2];
			                lastlink = link->links[M - 1];
			                i=M-2;
			                while(i>position){
			                        link->keys[i] = link->keys[i - 1];
			                        link->links[i + 1] = link->links[i];
			                        i--;
			                }
			                link->keys[position] = newKey;
			                link->links[position + 1] = newlink;
			        }
			        splitPosition = (M - 1) / 2;
			        (*upKey) = link->keys[splitPosition];
			
			        (*newnode) = new struct node;
			        link->n = splitPosition; 
			        (*newnode)->n = M - 1 - splitPosition;
			        i=0;
			        while (i < (*newnode)->n) {
			                (*newnode)->links[i] = link->links[i + splitPosition + 1];
			                if (i < (*newnode)->n - 1)
			                        (*newnode)->keys[i] = link->keys[i + splitPosition + 1];
			                else
			                        (*newnode)->keys[i] = lastKey;
			                i++;
			        }
			        (*newnode)->links[(*newnode)->n] = lastlink;
			        return Available;
		    	}
		}
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
		int searchPosition(int key, int *keyList, int n) {
		        int position = 0;
		        while (position < n && key > keyList[position])
		                position++;
		        return position;
		}
		
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
		        }
		}
		
		
		int del(struct node *link, int key) {
		        int position, i, pivot, n, min;
		        int *keyList;
		        int state;
		        struct node **links, *leftLink, *rightLink;
		
		        if (link == NULL)
		                return NotFound;
		        n = link->n;
		        keyList = link->keys;
		        links = link->links;
		        min = (M - 1) / 2;
		        position = searchPosition(key, keyList, n);
		        if (links[0] == NULL) {
		                if (position == n || key < keyList[position])
		                        return NotFound;
		                i = position + 1;
						while(i < n)
		                {
		                        keyList[i - 1] = keyList[i];
		                        links[i] = links[i + 1];
		                        i++;
		                }
		                return --link->n >= (link == root ? 1 : min) ? 3 : 4;
		        }
		        if (position < n && key == keyList[position]) {
		                struct node *tmpPtr = links[position], *tmpPtr1;
		                int nkey;
		                for(;;) {
		                        nkey = tmpPtr->n;
		                        tmpPtr1 = tmpPtr->links[nkey];
		                        if (tmpPtr1 == NULL)
		                                break;
		                        tmpPtr = tmpPtr1;
		                }
		                keyList[position] = tmpPtr->keys[nkey - 1];
		                tmpPtr->keys[nkey - 1] = key;
		        }
		        state = del(links[position], key);
		        if (state != LessKeys)
		                return state;
		
		        if (position > 0 && links[position - 1]->n > min) {
		                pivot = position - 1;
		                leftLink = links[pivot];
		                rightLink = links[position];
		                rightLink->links[rightLink->n + 1] = rightLink->links[rightLink->n];
		                i = rightLink->n;
		                while (i>0) {
		                        rightLink->keys[i] = rightLink->keys[i - 1];
		                        rightLink->links[i] = rightLink->links[i - 1];
		                        i--;
		                }
		                rightLink->n++;
		                rightLink->keys[0] = keyList[pivot];
		                rightLink->links[0] = leftLink->links[leftLink->n];
		                keyList[pivot] = leftLink->keys[--leftLink->n];
		                return Success;
		        }
		        if (position < n && links[position + 1]->n > min) {
		                pivot = position;
		                leftLink = links[pivot];
		                rightLink = links[pivot + 1];
		                leftLink->keys[leftLink->n] = keyList[pivot];
		                leftLink->links[leftLink->n + 1] = rightLink->links[0];
		                keyList[pivot] = rightLink->keys[0];
		                leftLink->n++;
		                rightLink->n--;
		                i=0;
		                while(i < rightLink->n) {
		                        rightLink->keys[i] = rightLink->keys[i + 1];
		                        rightLink->links[i] = rightLink->links[i + 1];
		                        i++;
		                }
		                rightLink->links[rightLink->n] = rightLink->links[rightLink->n + 1];
		                return VAP;
		        }
		
		        if (position == n)
		                pivot = position - 1;
		        else
		                pivot = position;
		
		        leftLink = links[pivot];
		        rightLink = links[pivot + 1];
		        leftLink->keys[leftLink->n] = keyList[pivot];
		        leftLink->links[leftLink->n + 1] = rightLink->links[0];
		        for (i = 0; i < rightLink->n; i++) {
		                leftLink->keys[leftLink->n + 1 + i] = rightLink->keys[i];
		                leftLink->links[leftLink->n + 2 + i] = rightLink->links[i + 1];
		        }
		        leftLink->n = leftLink->n + rightLink->n + 1;
		        delete rightLink;
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
		}	
};



void eatline(void) {
        char c;
        while ((c = getchar()) != '\n');
}
		

//int main() {
//        int key;
//        int choice;
//        printf("Creation of B tree for M=%d\n", M);
//        while (1) {
//        		btree b1;
//                printf("1.Insert\n");
//                printf("2.Delete\n");
//                printf("3.Search\n");
//                printf("4.Display\n");
//                printf("5.Quit\n");
//                printf("6.Enumerate\n");
//                printf("7.Total Keys\n");
//                printf("8.Min and Max Keys\n");
//                printf("Enter your choice : ");
//                scanf("%d", &choice); eatline();
//
//                switch (choice) {
//                case 1:
//                        printf("Enter the key : ");
//                        scanf("%d", &key); eatline();
//                        b1.Insert(key);
//                        break;
//                case 2:
//                        printf("Enter the key : ");
//                        scanf("%d", &key); eatline();
//                        b1.Delete(key);
//                        break;
//                case 3:
//                        printf("Enter the key : ");
//                        scanf("%d", &key); eatline();
//                        b1.search(key);
//                        break;
//                case 4:
//                        printf("Btree is :\n");
//                        b1.display(root, 0);
//                        break;
//                case 5:
//                        exit(1);
//                default:
//                        printf("Wrong choice\n");
//                        break;
//                }/*End of switch*/
//        }/*End of while*/
//        return 0;
//
//}/*End of main()*/
int main() {
   
}

