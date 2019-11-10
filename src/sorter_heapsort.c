#include <stdio.h>
#include <stdlib.h>
#include "../headers/minheap.h"
#include "../headers/record.h"

int main(int argc, char const *argv[])
{
    if (argc < 5)
        return 0;
    // Read arguments
    FILE *input = fopen(argv[1],"r");
    int firstRecord = atoi(argv[2]);
    int lastRecord = atoi(argv[3]);
    int field = atoi(argv[4]);
    // Create the min heap
    MinHeap heap = MinHeap_Create(lastRecord - firstRecord + 1,field);
    Record rec;
    char buf[Record_Size()];
    int i;
    // Read the records from the file
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        fread(buf,Record_Size(),1,input);
        Record_Init(&rec,buf);
        MinHeap_Insert(heap,rec);
    }
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        rec = MinHeap_ExtractMin(heap);
        Record_Print(rec);
        Record_Destroy(&rec);
    }
    MinHeap_Destroy(&heap);
    fclose(input);
    return 0;
}
