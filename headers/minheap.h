#ifndef MINHEAP_H
#define MINHEAP_H

#include "../headers/record.h"

typedef struct minheap *MinHeap;

MinHeap MinHeap_Create(int,int);
Record MinHeap_FindMin(MinHeap);
int MinHeap_Insert(MinHeap,Record);
Record MinHeap_ExtractMin(MinHeap);
int MinHeap_Destroy(MinHeap*);

#endif