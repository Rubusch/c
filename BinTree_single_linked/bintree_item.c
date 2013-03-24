// bintree_item.c

#include "bintree_item.h"
//#define DEBUG

static item* root = NULL;

void generateTree(char[]);
static item* getNewItem(char[]);
static void addLeftItem(item*, char[]);
static void addRightItem(item*, char[]);
static char* getContent(item*);
static item* getLeftItem(item*);
static item* getRightItem(item*);
void addItemSorted(char[]);
item* findItem(char[]);


void generateTree(char content[])
{
  root = getNewItem(content);  
}


static item* getNewItem(char content[])
{
  item* pItem = malloc(sizeof(item));
  if(pItem == NULL){
    fprintf(stderr, "bintree_item::getNewItem() - Bad allocation!\n");
    exit(8);
  }
 
  pItem->lhs = NULL;
  pItem->rhs = NULL;
  strcpy(getContent(pItem), content);

  return pItem;
}


static void addLeftItem(item* parent, char content[])
{
  parent->lhs = getNewItem(content);
}


static void addRightItem(item* parent, char content[])
{
  parent->rhs = getNewItem(content);
}


static char* getContent(item* node)
{
  return node->content;
}


static item* getLeftItem(item* node)
{
  return node->lhs;
}


static item* getRightItem(item* node)
{
  return node->rhs;
}

void addItemSorted(char content[])
{
#ifdef DEBUG
  printf("\nAdd a new item: \'%s\'\n", content); // XXX 
#endif

  item* pItem = root;
  if(pItem == NULL){
    generateTree(content);
    return;
  }

  int cnt = 0;
  int bAdded = 0;
  while(!bAdded){
    cnt = 0;
    while( (cnt < strlen(content)) && (cnt < strlen(getContent(pItem)))){
      if((cnt < strlen(content)-1) && (cnt < strlen(getContent(pItem))-1) 
	 && (content[cnt] == getContent(pItem)[cnt]) ){
	++cnt;
	continue;
      
      }else if( (content[cnt] < getContent(pItem)[cnt])
	    || (islower( content[cnt]) && isupper( getContent(pItem)[cnt])) ){
#ifdef DEBUG
	printf("content (\'%c\'=%i of \'%s\') < pItem->content (\'%c\'=%i of \'%s\')\t\t/\n", content[cnt], content[cnt], content, getContent(pItem)[cnt], getContent(pItem)[cnt], getContent(pItem)); // XXX
#endif

	  if(getLeftItem(pItem) == NULL){ 
	    addLeftItem(pItem, content);
	    bAdded = 1;
#ifdef DEBUG
	    printf("\t\'%s\' added.\n", content); // XXX
#endif
	    break;
	  }else{
	    pItem = getLeftItem(pItem); 
	    cnt = 0;
	  }

      }else if( (content[cnt] > getContent(pItem)[cnt])
		|| (isupper( content[cnt]) && islower( getContent(pItem)[cnt])) ){
#ifdef DEBUG
	printf("content (\'%c\'=%i of \'%s\') > pItem->content (\'%c\'=%i of \'%s\')\t\t\\\n", content[cnt], content[cnt], content, getContent(pItem)[cnt], getContent(pItem)[cnt], getContent(pItem)); // XXX 
#endif
	if(getRightItem(pItem) == NULL){
	  addRightItem(pItem, content);
	  bAdded = 1;
#ifdef DEBUG
	  printf("\t\'%s\' added.\n", content); // XXX
#endif
	  break;
	}else{ 
	  pItem = getRightItem(pItem);
	  cnt = 0;
	}

      }else{ // strings are equal or at least equal to the first part of the other string
#ifdef DEBUG
	printf("ELSE - strings \'%s\' and \'%s\' are equal\n", content, getContent(pItem));// XXX 
#endif
	if(strcmp(content, getContent(pItem)) >= 0){
#ifdef DEBUG
	  printf("\t\'%s\' is bigger than \'%s\'\t\t\\\n", content, getContent(pItem)); // XXX
#endif
	  if(pItem->rhs == NULL){ 
	    addRightItem(pItem, content);
	    bAdded = 1;
#ifdef DEBUG
	    printf("\t\'%s\' added.\n", content); // XXX
#endif
	    break;
	  }else{
	    pItem = getRightItem(pItem);
	    cnt = 0;
	  }
	}else{
#ifdef DEBUG
	  printf("\t\'%s\' is smaller or equal than \'%s\'\t\t/\n", content, getContent(pItem)); // XXX
#endif
	  if(pItem->lhs == NULL){ 
	    addLeftItem(pItem, content);
	    bAdded = 1;
#ifdef DEBUG
	    printf("\t\'%s\' added.\n", content); // XXX
#endif
	    break;
	  }else{
	    pItem = getLeftItem(pItem);
	    cnt = 0;
	  }
	}
      }
    } // while -> check letters
  } // while -> added
}
