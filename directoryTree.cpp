#include<iostream>
#include <vector>
#include <fstream>
using namespace std;


#define M 4
#define blockCount 100
#define blockSize 50

int currBtrfsKey=1000;


struct dir{
	string name;
	int btrfsKey;
	int size;
	bool file;
	vector<struct dir *> sub;
	vector<int> allocBlocks;
};

struct node
{
	bool leaf;
	int currKeys;
	struct node * parent;
	struct node * ptrArr[M];
	int Keys[M-1];
	vector<int> allocBlocks[M];
};

struct node  * ROOT=NULL; //B-Tree
struct dir root;//Directory Tree

bool blocks[blockCount];
struct dir * currDir;
struct dir * preDir;




/********************************   Disk 1   ******************************************/
char disk1[blockCount][blockSize];
/********************************   Disk 2   ******************************************/
char disk2[blockCount][blockSize];
/**************************************************************************************/



/*---------------------Simulation Of BTRFS-File System--------------------------------*/
void initStatusArr();
struct dir * createDirTree();
void mountDisks();
void ls();
void mkdir(string Name);
struct dir * cd(string Name);
void touch(string Name);
struct dir * fileLoc(string Name);
void alloc(struct dir * Loc,string text);
void write(string fileName,string text);
void writefromFile(string fileName,string source);
void rmdir(string Name);
void rm(string Name);
void read(string fileName);
void cp(string source,string destination);
void viewDisk();
void viewDetails(string fileName);
void help();
/*-------------------------------------------------------------------------------------*/

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


/************************ Implementation Of B-TREE *************************************/
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

/**************************************************************************************/
/**************************************************************************************/


/*****************       Simulation Of BTRFS-File System    ***************************/

//init blocks to free
void initStatusArr()
{
	for (int i=0;i<blockCount;i++)
	{
		blocks[i]=0;
	}
}

/* Create   directory  Tree*/
struct dir * createDirTree()
{
	root.btrfsKey=0;
	root.file=0;
	root.name="root";
	root.size=0;
	return &root;
}
/* Set 2 Disk To File System */
/* Add them to Directory Tree*/
void mountDisks()
{
	struct dir *disk1;
	struct dir *disk2;
	disk1=new struct dir;
	disk2=new struct dir;
	
	disk1->file=0;
	disk1->name="Disk1";
	disk1->size=0;
	disk2->file=0;
	disk2->name="Disk2";
	disk2->size=0;
	root.sub.push_back(disk1);
	root.sub.push_back(disk2);
}
/* Show the contents of current directory*/
void ls()
{
	for (int i=0;i<(currDir->sub).size();i++) {
		cout << (currDir->sub[i])->name<< " " ; 
	}
}

/* make new directory */
void mkdir(string Name)
{
	struct dir *tmp;
	tmp=new struct dir;
	tmp->file=0;
	tmp->name=Name;
	tmp->size=0;
	currDir->sub.push_back(tmp);
}
/* change directory*/
struct dir * cd(string Name){
	for (int i=0;i<(currDir->sub).size();i++) {
		if((currDir->sub[i])->name==Name)
		{
			return (currDir->sub[i]);
		}
	}
	cout << "There is no directory called '"<< Name <<"' ."<<endl;
	return currDir;
}
/* make files*/
void touch(string Name)
{
	struct dir *tmp;
	tmp=new struct dir;
	tmp->file=1;
	tmp->name=Name;
	tmp->size=0;
	tmp->btrfsKey=++currBtrfsKey;
	currDir->sub.push_back(tmp);
}
/* get the memory blocks of file or directory*/
struct dir * fileLoc(string Name)
{
	for (int i=0;i<(currDir->sub).size();i++) {
		if((currDir->sub[i])->name==Name)
		{
			return (currDir->sub[i]);
		}
	}
	cout << "There is no file called '"<< Name <<"' ."<<endl ;
	return currDir;
}
/* this function responsible for memory allocation on disks*/
void alloc(struct dir * Loc,string text)
{
	int ptr=0;
	int j;
	for(int i=0;i<blockCount;i++)//search for free blocks
	{
		j=0;
		if(blocks[i]==0)//when we found a free block
		{
			Loc->allocBlocks.push_back(i);//we allocate it to that file
			blocks[i]=1;
			while(ptr<text.size() && j<blockSize )
			{
				disk1[i][j]=text[ptr];
				disk2[i][j]=text[ptr];//raid 1 happends here
				ptr++;
				j++;
			}
			
			Loc->size+=j; // increrase the file size
			
			disk1[i][j]=EOF;
			disk2[i][j]=EOF;
			if(ptr>=text.size())
			{
				return;
			}
		}
	}
}
/* write into text file */
void write(string fileName,string text)
{
	struct dir * fLoc=fileLoc(fileName);
	if(currDir!=fLoc)
	{
	    alloc(fLoc,text);
	}
	else
	{
		cout<<"There are no file called ''"<<fileName<<"' ."<<endl;
	}
}
/* write into text file from out source */
void writefromFile(string fileName,string source)
{
	string line;
	struct dir * fLoc=fileLoc(fileName);
	if(currDir!=fLoc)
	{
		fstream file;
	  	file.open(source.c_str()); 
	  	if (file.is_open())
	  	{
	    	while ( getline (file,line) )
		    {
			    alloc(fLoc,line);
		    }
	    file.close();
		}
	  	else cout << " Unable to open file !";
	}
	 
}
/* remove directory*/
void rmdir(string Name)
{
	for (int i=0;i<(currDir->sub).size();i++) {
		if((currDir->sub[i])->name==Name)
		{
			struct dir * tmp=currDir->sub[i];
			(currDir->sub).erase((currDir->sub).begin()+i);
			delete tmp;
			return;
		}
		
	}
	cout << "There is no directory called '"<< Name <<"' ."<<endl ;
}
/*remove file*/
void rm(string Name)
{
	for (int i=0;i<(currDir->sub).size();i++) {
		if((currDir->sub[i])->name==Name)
		{
			struct dir * tmp=currDir->sub[i];
			(currDir->sub).erase((currDir->sub).begin()+i);
			delete tmp;
			return;
		}
	}
	cout << "There is no file called '"<< Name <<"' ."<<endl ;	
}
/* read file*/
void read(string fileName)
{
	struct dir * fLoc=fileLoc(fileName);
	if(currDir!=fLoc)
	{
		string text;
		for (int i=0;i<(fLoc->allocBlocks.size());i++) 
		{
			int j=0;
			while(disk1[fLoc->allocBlocks[i]][j]!=EOF && j<blockSize)
			{
				text+=disk1[fLoc->allocBlocks[i]][j];
				j++;
			}
		}
		cout << text;
	}
}
/* copy file into another file*/
void cp(string source,string destination)
{
	struct dir * sLoc=fileLoc(source);
	struct dir * dLoc=fileLoc(destination);
	dLoc->allocBlocks.clear();
	if(currDir!=sLoc && currDir!=dLoc)
	{
		int ptr,j;
		for (int i=0;i<(sLoc->allocBlocks.size());i++) 
		{
			for(int k=0;k<blockCount;k++)//search for free blocks
			{
				j=0;
				if(blocks[k]==0)//when we found a free block
				{
					dLoc->allocBlocks.push_back(k);//we allocate it to that file
					blocks[k]=1;
					while(ptr<blockSize && j<blockSize )
					{
						disk1[k][j]=disk1[sLoc->allocBlocks[i]][j];
						disk2[k][j]=disk2[sLoc->allocBlocks[i]][j];//raid 1 happends here

						j++;
					}
					break;
				}
			}
		}
		dLoc->size=sLoc->size;
	}
}
/* show bloack allocation for disk */
void viewDisk()
{
	cout<<endl<<endl<<"Number Of BLOCKS In Each DISK : "<<blockCount<<endl<<"Number of Bytes in each BLOCK : "<<blockSize<<"Bytes"<<endl;
	cout<<"Total CAPACITY : "<<blockCount*blockSize<<" Bytes"<<endl<<endl;
	
	int count=0;
	for(int i=0;i<blockCount;i++)
	{
		cout<<"Block "<<i<<"  \t\t";
		if(blocks[i])
		{
			count++;
			cout<<"[ALLOCATED] "<<endl;
		}
		else
		{
			cout<<"[ F R E E ]"<<endl;
		}
	}
	cout<<endl<<count<<"-BLOCKS ALLOCATED\t"<<blockCount-count<<"-BLOCKS FREE"<<endl;
	cout<<endl<<count*blockSize<<" Bytes USED\t\t"<<(blockCount-count)*blockSize<<" Bytes FREE"<<endl;
}
void viewDetails(string fileName)
{
	struct dir * p=fileLoc(fileName);
	if(p->file==1)
	{
		cout<<endl;
		cout<<"File  Name : "<< p->name<<endl;
		cout<<"File  Size : "<< p->size<<endl;
		cout<<"BTRFS Index: "<< p->btrfsKey<<endl;
		cout<<"ALLOCATED BLOCKS : ";
		for (int i=0;i<(p->allocBlocks.size());i++) 
		{
			cout << p->allocBlocks[i]<<" ";
		}
		cout<<endl;
	}
	else
	{
		cout<<endl;
		cout<<"DIR  Name : "<< p->name<<endl;
		cout<<"SUB DIR : ";
		for (int i=0;i<(p->sub.size());i++) 
		{
			cout << p->sub[i]<<" ";
		}
		cout<<endl;
	}
	
}
void help()
{
	cout<<endl;
	cout<< "ls"<<"\t List  information  about the FILE/DIRs (in the current directory)."<<endl;
	cout<< "cd"<<"\t Change the shell working directory. [eg:- cd Disk1]"<<endl;
	cout<< "mkdir"<<"\t Make directories. [eg:- mkdir folder]"<<endl;
	cout<< "rm" << "\t Remove (unlink) the FILE(s). [eg:- rm test.txt]"<<endl;
	cout<< "rmdir" << "\t Remove empty directories. [eg:- rmdir floder]"<<endl;
	cout<< "touch" << "\t Change file timestamps. [eg:- touch test.txt]"<<endl;
	cout<< "write" << "\t Write in to a TEXT file. [eg:- write test.txt]"<<endl;
	cout<< "swrite" << "\t Automatically Write in to a TEXT file using another SOURCE. [eg:- write test.txt source.txt]"<<endl;
	cout<< "  \t (This Source file must be near the 18001238.cpp)"<<endl;
	cout<< "info" << "\t Show the all DATA about file including (BTRFS TREE INDEX,ALLOCATED BLOCKS). [eg:- info test.txt]"<<endl;
	cout<< "viewdisk"<< " Show the all BLOCK ALOCATION IN DISK1. [eg:- viewdisk]"<<endl;
	cout<< "clear" <<" \t Clear the TERMINAL."<<endl<<endl;
}
int main()
{
	string locationString="BTRFS-fileSystem:~";
	string prevlocationString;
	cout<<      "Type 'commands' to view all commands.";
	cout<<endl<<"-------------------------------------"<<endl;
	initStatusArr();
	currDir=createDirTree();
	mountDisks();
	string cmd,pr1,pr2;
	while(1)
	{
		cout<<locationString<<"$ ";
		cin>>cmd;
		if(cmd=="ls")
		{
			ls();
			cout<<endl;
		}
		else if(cmd=="mkdir")
		{
			cin>>pr1;
			mkdir(pr1);
		}
		else if(cmd=="cd")
		{
			cin>>pr1;
			preDir=currDir;
			currDir=cd(pr1);
			if(currDir!=preDir)
				locationString=locationString+"/"+pr1;	
		}
		else if(cmd=="touch")
		{
			cin>>pr1;
			touch(pr1);
		}
		else if(cmd=="write")
		{
			cin>>pr1;
			cout<<"# lines in TEXT : ";
			int n;
			cin>>n;
			string text;
			string line;
			for (int i=0;i<n;i++)
		    {
			    cin >> line;
			    text = text+line+"\n";
		    }
			write(pr1,text);
		}
		else if(cmd=="swrite")
		{
			cin>>pr1>>pr2;
			writefromFile(pr1,pr2);
		}
		else if(cmd=="rmdir")
		{
			cin>>pr1;
			rmdir(pr1);
		}
		else if(cmd=="rm")
		{
			cin>>pr1;
			rm(pr1);
		}
		else if(cmd=="read")
		{
			cin>>pr1;
			read(pr1);
			cout<<endl;
		}
		else if(cmd=="cp")
		{
			cin>>pr1>>pr2;
			cp(pr1,pr2);
		}
		else if(cmd=="viewdisk")
		{
			viewDisk();
		}
		else if(cmd=="info")
		{
			cin>>pr1;
			viewDetails(pr1);
		}
		else if(cmd=="clear")
		{
			system("CLS");
		}
		else if(cmd=="commands")
		{
			help();
		}
		else{
			cout<<"Command ''"<<cmd<<"' not found, type commands to view all commands."<<endl;
		}
		//else if()
	}
	return 0;
}

/**************************************************************************************/
/* Index Number : 18001238      ASSIGNMENT B     BTRFS-FILE SYSTEM     PERERA R.D     */
/**************************************************************************************/
