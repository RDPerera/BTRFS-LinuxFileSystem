#include<iostream>
#include <vector>
#include <fstream>
using namespace std;

#define blockCount 100
#define blockSize 50

int currBtrfsKey=1000;

bool blocks[blockCount];
struct dir * currDir;
struct dir * preDir;

/*************************   Disk 1   ******************************************/
char disk1[blockCount][blockSize];
/*************************   Disk 2   ******************************************/
char disk2[blockCount][blockSize];
/*******************************************************************************/

void initStatusArr()
{
	for (int i=0;i<blockCount;i++)
	{
		blocks[i]=0;
	}
}

struct dir{
	string name;
	int btrfsKey;
	int size;
	bool file;
	vector<struct dir *> sub;
	vector<int> allocBlocks;
};
struct dir root;

struct dir * createDirTree()
{
	root.btrfsKey=0;
	root.file=0;
	root.name="root";
	root.size=0;
	return &root;
}
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
void ls()
{
	cout << endl;
	for (int i=0;i<(currDir->sub).size();i++) {
		cout << (currDir->sub[i])->name<< " " ; 
	}
	cout << endl;
}
void mkdir(string Name)
{
	struct dir *tmp;
	tmp=new struct dir;
	tmp->file=0;
	tmp->name=Name;
	tmp->size=0;
	currDir->sub.push_back(tmp);
}

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
void write(string fileName)
{
	struct dir * fLoc=fileLoc(fileName);
	if(currDir!=fLoc)
	{
		string text;
		string line;
		do
	    {
		    getline(cin, line);
		    text += line;
	    }
	    while(line != "");
	    alloc(fLoc,text);
	}   
}
void writefromFile(string fileName,string source)
{
	string line;
	struct dir * fLoc=fileLoc(fileName);
	if(currDir!=fLoc)
	{
		cout << "file Correct"<<endl;
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
void rmdir(string Name)
{
	for (int i=0;i<(currDir->sub).size();i++) {
		if((currDir->sub[i])->name==Name)
		{
			struct dir * tmp=currDir->sub[i];
			(currDir->sub).erase((currDir->sub).begin()+i);
			delete tmp;
		}
		
	}
	cout << "There is no directory called '"<< Name <<"' ."<<endl ;
}

void rm(string Name)
{
	for (int i=0;i<(currDir->sub).size();i++) {
		if((currDir->sub[i])->name==Name)
		{
			struct dir * tmp=currDir->sub[i];
			(currDir->sub).erase((currDir->sub).begin()+i);
			delete tmp;
		}
	}
	cout << "There is no file called '"<< Name <<"' ."<<endl ;	
}

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
void viewDisk()
{
	cout<<endl<<endl<<"Number Of BLOCKS In Each DISK : "<<blockCount<<" \t Number of Bytes in each BLOCK : "<<blockSize<<"Bytes";
	cout<<endl<<"X - Byte Allocated \t O-Byte Free";
	cout<<endl<<"         \t|";
	int i,j;
	for(j=0;j<blockSize;j++)
		cout<<"--";
	cout<<"-|"<<endl;
	for(i=0;i<blockCount;i++)
	{
		cout<<"Block "<<i<<"  \t";
		if(blocks[i])
		{
			cout<<"[ ";
			for(j=0;j<blockSize;j++)
			{
				cout<<"X ";
			}
			cout<<"]"<<endl;
		}
		else
		{
			cout<<"[ ";
			for(j=0;j<blockSize;j++)
			{
				cout<<"O ";
			}
			cout<<"]"<<endl;
		}
	}
	cout<<"         \t|";
	for(j=0;j<blockSize;j++)
		cout<<"--";
	cout<<"-|"<<endl;
}
int main()
{

	string locationString="root";
	initStatusArr();
	currDir=createDirTree();
	mountDisks();
	ls();
	currDir=cd("Disk1");
	mkdir("Folder1");
	ls();
	touch("test1.txt");
	touch("test2.txt");
	touch("test3.txt");
	writefromFile("test1.txt","source.txt");
	//writefromFile("test2.txt","source.txt");
	writefromFile("test3.txt","source.txt");
	cout<<endl;
	cp("test1.txt","test2.txt");
	ls();
	read("test1.txt");
	cout << endl << endl;
	read("test2.txt");
	cout << endl << endl;
	read("test3.txt");
	viewDisk();
	return 0;
}
