#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
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
    // Usage: ./quicksort <inputfile> <first_record> <last_record> <column> <output_file>
    if (argc < 6)
        exit(1);
    FILE *input;
    if ((input = fopen(argv[1],"r")) == NULL) {
        fprintf(stderr,"File %s error\n",argv[2]);
        exit(1);
    }
    int firstRecord = atoi(argv[2]);
    int lastRecord = atoi(argv[3]);
    int field = atoi(argv[4]);
    Record *records;
    if ((records = malloc((lastRecord - firstRecord + 1) * sizeof(Record))) == NULL) {
        fprintf(stderr,"Not enough memeory\n");
        exit(1);
    }
    char buf[Record_Size()];
    int i;
    // Seek to starting record
    fseek(input,(long int)((firstRecord - 1)*Record_Size()),SEEK_SET);
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        fread(buf,Record_Size(),1,input);
        Record_Init(records + i,buf);
    }
    fclose(input);
    quicksort(records,0,lastRecord - firstRecord,field);
    int fd = open(argv[5],O_WRONLY);
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        //Record_Print(records[i]);
        write(fd,records[i],Record_Size());
        Record_Destroy(records + i);
    }
    close(fd);
    free(records);
    // Send SIGUSR2 to the parent
    kill(getppid(),SIGUSR2);
    return EXIT_SUCCESS;
}
