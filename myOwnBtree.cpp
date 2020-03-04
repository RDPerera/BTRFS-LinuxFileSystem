#include<iostream>
#include<vector>
#define M 4

using namespace std;
struct node
{
	bool leaf;
	int currKeys;
	struct node * parent;
	struct node * ptrArr[M];
	int Keys[M-1];
	vector<int> allocBlocks[M];
};

struct node  * ROOT=NULL;

/*-------------------------Implementation Of B-TREE-----------------------------------*/
void createTree();
void split(struct node  * ptr,int key,vector<int> allocatedBlocks);
void insertIt(struct node  * ptr,int key,vector<int> allocatedBlocks);
struct node  * correctPos(struct node  * curr,int key);
void insert(int key,vector<int> allocatedBlocks);
struct node  *  successor(struct node  * curr);
struct node  * search(struct node  * curr,int key);
struct node  *  leftSib(struct node  * curr);
struct node  *  rightSib(struct node  * curr);
void Delete(int key);
/*-------------------------------------------------------------------------------------*/


void createTree()
{
	ROOT=new struct node;
	ROOT->leaf=1;
	ROOT->currKeys=0;
}

void insert(int key,vector<int> allocatedBlocks)
{
	while(1)
	{
		 /*if tree is null create tree*/
		if(ROOT==NULL)
		{
			createTree(); 
		}
		else
		{
			/*Find correct position in the node to insert Key;*/
			struct node  * pos = correctPos(ROOT,key);
			if(pos->currKeys < M-1)//if node is not full
			{
				//insert the key into node and leave
				insertIt(pos,key,allocatedBlocks);
				return;
			}
			else//node is full
			{
				//split node into two parts and connect
				split(pos,key,allocatedBlocks);
			}
		}
	}
	
}

void Delete(int key)
{
	struct node  * ptr=search(ROOT,key); //find the node of key
	int index;
	if(ptr!=NULL)
	{
		for (int i=0;i < ptr->currKeys ;i++)//find the node position
		{
			if(ptr->Keys[i]==key)
			{
				index=i;
			}
					
		}
		if(ptr->leaf==0)//if node is not a leaf
		{
			/* find the closest successor and copy it to key */
			/* and delete  successor from node*/
			struct node  * scr=successor(ptr->ptrArr[index+1]);
			ptr->Keys[index]=scr->Keys[0];
			ptr->allocBlocks[index]=scr->allocBlocks[0];
			ptr=scr;
			for (int j = ptr->currKeys-1; j > 0; j--)
			{
				ptr->Keys[j - 1] = ptr->Keys[j];
				ptr->allocBlocks[j - 1] = ptr->allocBlocks[j];
			}
			ptr->currKeys=ptr->currKeys-1;
		}
		else
		{
			/**delete the key**/
			for (int j = ptr->currKeys-1; j > index; j--)
			{
				ptr->Keys[j - 1] = ptr->Keys[j];
				ptr->allocBlocks[j - 1] = ptr->allocBlocks[j];
			}
			ptr->currKeys=ptr->currKeys-1;	
		}
		while(1)
		{
			if(ptr->currKeys >= M/2-1)//node is not underflow
			{
				return;
			}
			struct node  * ls=leftSib(ptr);
			struct node  * rs=rightSib(ptr);
			int rightSibState=0;
			if(rs!=NULL)
				if(rs->currKeys>M/2-1)
					rightSibState=1;
			int leftSibState=0;
			if(ls!=NULL)
				if(ls->currKeys>M/2-1)
					leftSibState=1;
					
			/* if there are sibling with sufficiant keys */
			/*       redistribute the keys               */
			
			else if(rightSibState || leftSibState)
			{
				if(leftSibState && !(rightSibState))
				{
					ptr->currKeys=ptr->currKeys+1;
					for (int j = ptr->currKeys-1; j > 0; j--)
					{
						ptr->Keys[j] = ptr->Keys[j - 1];
						ptr->allocBlocks[j] = ptr->allocBlocks[j - 1];
					}
					int index;
					for(int i=0;i<ptr->parent->currKeys;i++)
					{
						if(ptr->parent->ptrArr[i]==ptr)
						{
							index=i;
							break;
						}
					}
					ptr->Keys[0] = ptr->parent->Keys[index];
					ptr->allocBlocks[0] = ptr->parent->allocBlocks[index];
					ptr->parent->Keys[index]=ls->Keys[ls->currKeys-1];
					ptr->parent->allocBlocks[index]=ls->allocBlocks[ls->currKeys-1];
					ls->currKeys=ls->currKeys-1;
				}
				if(rightSibState && !(leftSibState))
				{
					
					
					int index;
					for(int i=0;i<ptr->parent->currKeys;i++)
					{
						if(ptr->parent->ptrArr[i]==ptr)
						{
							index=i;
							break;
						}
					}
					ptr->Keys[ptr->currKeys] = ptr->parent->Keys[index];
					ptr->allocBlocks[ptr->currKeys] = ptr->parent->allocBlocks[index];
					ptr->currKeys=ptr->currKeys+1;
					
					ptr->parent->Keys[index]=rs->Keys[0];
					ptr->parent->allocBlocks[index]=rs->allocBlocks[0];
					
					for (int j = rs->currKeys; j > 0; j--)
					{
						rs->Keys[j - 1] = rs->Keys[j];
						rs->allocBlocks[j - 1] = rs->allocBlocks[j];
					}
					rs->currKeys=rs->currKeys-1;
				}
				if(leftSibState && rightSibState)
				{
					ptr->currKeys=ptr->currKeys+1;
					for (int j = ptr->currKeys-1; j > 0; j--)
					{
						ptr->Keys[j] = ptr->Keys[j - 1];
						ptr->allocBlocks[j] = ptr->allocBlocks[j - 1];
					}
					int index;
					for(int i=0;i<ptr->parent->currKeys;i++)
					{
						if(ptr->parent->ptrArr[i]==ptr)
						{
							index=i;
							break;
						}
					}
					ptr->Keys[0] = ptr->parent->Keys[index];
					ptr->allocBlocks[0] = ptr->parent->allocBlocks[index];
					ptr->parent->Keys[index]=ls->Keys[ls->currKeys-1];
					ptr->parent->allocBlocks[index]=ls->allocBlocks[ls->currKeys-1];
					ls->currKeys=ls->currKeys-1;
				}
				return;
			}
			/* if parent is root */
			else if(ptr->parent==ROOT)
			{
				/* if parent is root and it has only one key*/
				/*        then marge ROOT,SIBLING,NODE      */
				if(ptr->parent->currKeys==1)
				{
					if(ls==NULL)
					{
						ptr->Keys[ptr->currKeys]=ptr->parent->Keys[0];
						ptr->allocBlocks[ptr->currKeys]=ptr->parent->allocBlocks[0];
						ptr->ptrArr[ptr->currKeys+1]=rs->ptrArr[0];
						ptr->currKeys=ptr->currKeys+1;
						for(int i=0;i<rs->currKeys;i++)
						{
							ptr->Keys[i+ptr->currKeys]=rs->Keys[i];
							ptr->allocBlocks[i+ptr->currKeys]=rs->allocBlocks[i];
							ptr->ptrArr[i+1+ptr->currKeys]=rs->ptrArr[i+1];
						}
						ptr->currKeys=ptr->currKeys+ls->currKeys;
					}
					if(rs==NULL)
					{
						ls->Keys[ls->currKeys]=ptr->parent->Keys[0];
						ls->allocBlocks[ls->currKeys]=ptr->parent->allocBlocks[0];
						ls->ptrArr[ls->currKeys+1]=ptr->ptrArr[0];
						ls->currKeys=ls->currKeys+1;
						for(int i=0;i<ptr->currKeys;i++)
						{
							ls->Keys[i+ls->currKeys]=ptr->Keys[i];
							ls->allocBlocks[i+ls->currKeys]=ptr->allocBlocks[i];
							ls->ptrArr[i+1+ls->currKeys]=ptr->ptrArr[i+1];
						}
						ls->currKeys=ls->currKeys+ptr->currKeys;
					}
				}
				/* if parent is root and it has more than one key*/
				else
				{
					if(ls==NULL)
					{
						for(int i=0;i<rs->currKeys;i++)
						{
							ptr->Keys[i+ptr->currKeys]=rs->Keys[i];
							ptr->allocBlocks[i+ptr->currKeys]=rs->allocBlocks[i];
							ptr->ptrArr[i+1+ptr->currKeys]=rs->ptrArr[i+1];
						}
						ptr->currKeys=ptr->currKeys+ls->currKeys;
					}
					if(rs==NULL)
					{
						for(int i=0;i<ptr->currKeys;i++)
						{
							ls->Keys[i+ls->currKeys]=ptr->Keys[i];
							ls->allocBlocks[i+ls->currKeys]=ptr->allocBlocks[i];
							ls->ptrArr[i+1+ls->currKeys]=ptr->ptrArr[i+1];
						}
						ls->currKeys=ls->currKeys+ptr->currKeys;
					}
				}
			}
			
			/*              else marge SIBLING,NODE          */
			else
			{
				if(ls==NULL)
					{
						for(int i=0;i<rs->currKeys;i++)
						{
							ptr->Keys[i+ptr->currKeys]=rs->Keys[i];
							ptr->allocBlocks[i+ptr->currKeys]=rs->allocBlocks[i];
							ptr->ptrArr[i+1+ptr->currKeys]=rs->ptrArr[i+1];
						}
						ptr->currKeys=ptr->currKeys+ls->currKeys;
					}
					if(rs==NULL)
					{
						for(int i=0;i<ptr->currKeys;i++)
						{
							ls->Keys[i+ls->currKeys]=ptr->Keys[i];
							ls->allocBlocks[i+ls->currKeys]=ptr->allocBlocks[i];
							ls->ptrArr[i+1+ls->currKeys]=ptr->ptrArr[i+1];
						}
						ls->currKeys=ls->currKeys+ptr->currKeys;
					}	
			}
			ptr=ptr->parent;
		}
	}
}
/*This function will insert key to any node we provide*/
void insertIt(struct node  * ptr,int key,vector<int> allocatedBlocks)
{
	int flag=0;
	for (int i=0;i<ptr->currKeys;i++)
	{
		if(ptr->Keys[i] > key)
		{
			flag=1;
			ptr->currKeys=ptr->currKeys+1;
			for (int j = ptr->currKeys-1; j > i; j--)
			{
				ptr->Keys[j] = ptr->Keys[j - 1];
				ptr->allocBlocks[j] = ptr->allocBlocks[j - 1];
			}
        	ptr->Keys[i]=key;
        	ptr->allocBlocks[i]=allocatedBlocks;
        	return;
		}
	}
	if(flag==0)
	{
		ptr->currKeys=ptr->currKeys+1;
    	ptr->Keys[ptr->currKeys-1]=key;
    	ptr->allocBlocks[ptr->currKeys-1]=allocatedBlocks;
	}
}
/*This function split node into two parts and distribute keys among them*/
void split(struct node  * ptr,int key,vector<int> allocatedBlocks)
{
	struct node * node1=new struct node;
	struct node * node2=new struct node;
	*node1=*ptr;
	int k=node1->Keys[M-2];
	vector<int> k_blocks=node1->allocBlocks[M-2];
	node1->currKeys=node1->currKeys-1;
	node2->currKeys=1;
	node2->Keys[0]=key;
	node2->allocBlocks[0]=allocatedBlocks;
	node2->leaf=ptr->leaf;
	if(ptr==ROOT)
	{
		struct node * newRoot=new struct node;
		node1->parent=newRoot;
		node2->parent=newRoot;
		newRoot->leaf=0;
		newRoot->currKeys=1;
		newRoot->Keys[0]=k;
		newRoot->allocBlocks[0]=k_blocks;
		newRoot->ptrArr[0]=node1;
		newRoot->ptrArr[1]=node2;
		ROOT=newRoot;
	}
	else
	{
		ptr=ptr->parent;
	}
	
}
/*this functions helps insert to get the correct node to insert new value*/
struct node  * correctPos(struct node  * curr,int key)
{
	if(curr->leaf)
	{
		return curr;
	}
	else{
		for (int i=0;i<curr->currKeys;i++)
		{
			if(i==0 && curr->Keys[i]>key)
			{
				correctPos(curr->ptrArr[0],key);
			}
			else if(i==curr->currKeys-1 && curr->Keys[i]<key)
			{
				correctPos(curr->ptrArr[curr->currKeys],key);
			}
			else
			{
				if(curr->Keys[i+1]>key && curr->Keys[i]<key)
				{
					correctPos(curr->ptrArr[i+1],key);
				}
			}
		}
	}
}
/*find the closest successor of given B-tree*/
struct node  *  successor(struct node  * curr)
{
	if(curr->leaf)
	{
		return curr;
	}
	else
	{
		successor(curr->ptrArr[0]);
	}
}

/*Fine any given key from the B-Tree and return it's node pointer */
struct node  * search(struct node  * curr,int key)
{
	for (int i=0;i<curr->currKeys;i++)
		{
			if(i==0 && curr->Keys[i]<key && curr->leaf==0)
			{
				search(curr->ptrArr[0],key);
			}
			else if(i==curr->currKeys-1 && curr->Keys[i]<key  && curr->leaf==0)
			{
				search(curr->ptrArr[curr->currKeys],key);
			}
			else
			{
				if(curr->Keys[i]==key)
				{
					return curr;
				}
				else if(curr->Keys[i+1]>key && curr->Keys[i]<key  && curr->leaf==0)
				{
					search(curr->ptrArr[i+1],key);
				}
				else
				{
					cout<<"Key Not Found";
					return NULL;
				}
			}
		}
}
/*return the left sbling's pointer */
struct node  *  leftSib(struct node  * curr)
{
	int index;
	for(int i=0;i<curr->parent->currKeys;i++)
	{
		if(curr->parent->ptrArr[i]==curr)
		{
			index=i;
			break;
		}
	}
	if(index==0)
	{
		return NULL;
	}
	else
	{
		return curr->parent->ptrArr[index-1];
	}
}
/*return the right sbling's pointer */
struct node  *  rightSib(struct node  * curr)
{
	int index;
	for(int i=0;i<curr->parent->currKeys;i++)
	{
		if(curr->parent->ptrArr[i]==curr)
		{
			index=i;
			break;
		}
	}
	if(index==M)
	{
		return NULL;
	}
	else
	{
		return curr->parent->ptrArr[index+1];
	}
}


