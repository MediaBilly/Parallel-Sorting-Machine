#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../headers/minheap.h"
#include "../headers/record.h"

struct minheap
{
    int size;
    int last;
    int orderField;
    Record *Arr;
};

void swap(MinHeap heap,int x,int y)
{
    Record tmp = heap->Arr[x];
    heap->Arr[x] = heap->Arr[y];
    heap->Arr[y] = tmp;
}

MinHeap MinHeap_Create(int size,int orderField)
{
    // Allocate memory for the heap
    MinHeap heap = (MinHeap)malloc(sizeof(struct minheap));
    if (heap != NULL) {
        // Initialize variables
        heap->size = size;
        heap->last = 0;
        heap->orderField = orderField;
        // Allocate memory for the heap array
        heap->Arr = (Record*)malloc(size * sizeof(Record));
    } else {
        printf("Not enough memory for heap\n");
        return 0;
    }
    return heap;
}

Record MinHeap_FindMin(MinHeap heap)
{
    return heap->Arr[0];
}

int MinHeap_Insert(MinHeap heap,Record value)
{
    // No more elements fit
    if (heap->last == heap->size)
        return 0;
    // Insert to the last element position
    int i = heap->last++;
    int parent = (i-1) >> 1;
    heap->Arr[i] = value;
    // Move up to the appropriate position (parent must be <= and all children must be >= to it)
    while (i > 0 && Record_Compare(heap->Arr[i],heap->Arr[parent],heap->orderField))
    {
        swap(heap,i,parent);
        i = parent;
        parent = (i-1) >> 1;
    }
    return 1;
}

Record MinHeap_ExtractMin(MinHeap heap)
{
    // No elements to delete(empty heap)
    if (heap->last <= 0)
        return NULL;
    // Delete min element(root)
    Record ret = heap->Arr[0];
    // PLace last element to it's place and decrease last pointer by 1
    heap->Arr[0] = heap->Arr[--heap->last];
    int i = 0,left = 2*i + 1,right = 2*i + 2,min;
    // Min heapify starting at root to bring the last element to the correct position
    while (i < heap->last)
    {
        min = i;
        if (left <= heap->last && Record_Compare(heap->Arr[left],heap->Arr[min],heap->orderField))
            min = left;
        if (right <= heap->last && Record_Compare(heap->Arr[right],heap->Arr[min],heap->orderField))
            min = right;
        if (min != i)
        {
            swap(heap,i,min);
            i = min;
            left = 2*i + 1;
            right = 2*i + 2;
        }
        else
            break;
    }
    return ret;
}

int MinHeap_Destroy(MinHeap *heap)
{
    // Free all allocated memory
    if (heap != NULL)
    {
        free((*heap)->Arr);
        free(*heap);
        *heap = NULL;
        return 1;
    }
    return 0;
}