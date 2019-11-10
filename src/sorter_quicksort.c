#include <stdio.h>
#include <stdlib.h>
#include "../headers/record.h"

int partition(Record *records,int left,int right,int field)
{
    int i,j;
    Record tmp;
    i = left - 1;
    for (j = left; j < right; j++)
    {
        if (Record_Compare(records[j],records[right],field))
        {
            i++;
            tmp = records[i];
            records[i] = records[j];
            records[j] = tmp;
        }
    }
    tmp = records[i+1];
    records[i+1] = records[right];
    records[right] = tmp;
    return i+1;
}

void quicksort(Record *records,int left,int right,int field)
{
    if (left < right)
    {
        int q = partition(records,left,right,field);
        quicksort(records,left,q-1,field);
        quicksort(records,q+1,right,field);
    }
}

int main(int argc, char const *argv[])
{
    if (argc < 5)
        return 0;
    FILE *input = fopen(argv[1],"r");
    int firstRecord = atoi(argv[2]);
    int lastRecord = atoi(argv[3]);
    int field = atoi(argv[4]);
    Record *records = malloc((lastRecord - firstRecord + 1) * sizeof(Record));
    char buf[Record_Size()];
    int i;
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        fread(buf,Record_Size(),1,input);
        Record_Init(records + i,buf);
    }
    quicksort(records,firstRecord - 1,lastRecord - 1,field);
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        Record_Print(records[i]);
        Record_Destroy(records + i);
    }
    free(records);
    return 0;
}
