// dequeue_item.c

#include "dequeue_item.h"

static item* first;
static item* last;

static item* getNextItem(item*);
static item* getPrevItem(item*);


item* getNewItem(char content[])
{
  item* pItem = malloc(sizeof(item));
  if(pItem == NULL){
    fprintf(stderr, "dequeue_item::getNewItem() - bad allocation!\n");
    exit(8);
  }
  strcpy(pItem->content, content);

  return pItem;
};


void addItem(char content[])
{
  if(first == NULL){ 
    last = getNewItem(content);
    first = last;
  }else{
    item* pItem = getNewItem(content); 
    last->next = pItem;
    pItem->prev = last;
    last = last->next;
  }
};


void removeItemAt(int idx)
{
  if((idx < 0) || (idx >= dequeueSize())) return;

  item* pItem = getItemAt(idx);

  item* pBefore = NULL;
  if(pItem != first) pBefore = getPrevItem(pItem);

  item* pAfter = NULL;
  if(pItem != last) pAfter = getNextItem(pItem);


  if((pBefore != NULL) || (pAfter != NULL)){
    pBefore->next = pAfter;
    pAfter->prev = pBefore;
  }else if(pBefore == NULL){ 
    first = getNextItem(first);
    first->prev = NULL;    
  }else if(pAfter == NULL){
    last = getPrevItem(last);
    last->next = NULL;
  }

  free(pItem);
  pItem = NULL;
};


void insertItemAt(int idx, char content[])
{
  item* pItem = getNewItem(content);  
  if(idx <= 0){
    pItem->next = first;
    first->prev = pItem;
    first = pItem; 
    return;
  }

  if(idx >= dequeueSize()){
    addItem(content);
    return;
  }

  item* pBefore = getItemAt(idx-1);
  item* pAfter = getItemAt(idx);

  pBefore->next = pItem;
  pItem->prev = pBefore;

  pItem->next = pAfter;
  pAfter->prev = pItem;
};


int dequeueSize()
{
  if(first == NULL) return 0;

  item* pItem = first;
  int cnt=0;
  for(cnt=0; pItem != NULL; ++cnt, pItem = pItem->next)
    ;

  return cnt;
};


item* getItemAt(int idx)
{
  int size = dequeueSize();
  if((idx >= size) || (idx < 0)) return NULL;
  

  item* pItem = NULL;
  if(idx+1 <= size/2){
    pItem = first;
    int cnt = 0;
    for(cnt = 0; (cnt != idx) && (cnt < size); ++cnt)
      pItem = getNextItem(pItem);

  }else{
    pItem = last;
    int cnt = size-1;
    for(cnt = size-1; (cnt != idx) && (cnt >= 0); --cnt){
      pItem = getPrevItem(pItem);
    }
  }

  return pItem;
};


char* getContentAt(int idx)
{
  item* pItem = getItemAt(idx);
  if(pItem == NULL) return NULL;
  else return pItem->content;
};


int find(char content[], int length)
{
  item* pItem = first;
  int cnt = 0;
  while(pItem != NULL){
    if(!strcmp(pItem->content, content)){
      int i=0;
      for(i=0; i<length; ++i)
	if(pItem->content[i] != content[i]) break;
      
      if(i == length) return cnt;
    }
    pItem = pItem->next;
    ++cnt;
  }
  return -1;
};


static item* getNextItem(item* pItem)
{
  if(pItem == NULL) return NULL;
  return pItem->next;
};


static item* getPrevItem(item* pItem)
{
  if(pItem == NULL) return NULL;
  return pItem->prev;
};
