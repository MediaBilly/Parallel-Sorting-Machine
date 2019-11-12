#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "../headers/minheap.h"
#include "../headers/record.h"

int main(int argc, char const *argv[])
{
    if (argc < 5)
        return 0;
    // Read arguments
    FILE *input;
    if ((input = fopen(argv[1],"r")) == NULL) {
        fprintf(stderr,"File %s error\n",argv[2]);
        exit(1);
    }
    int firstRecord = atoi(argv[2]);
    int lastRecord = atoi(argv[3]);
    int field = atoi(argv[4]);
    // Create the min heap
    MinHeap heap = MinHeap_Create(lastRecord - firstRecord + 1,field);
    Record rec;
    char buf[Record_Size()];
    int i;
    // Seek to starting record
    fseek(input,(long int)((firstRecord - 1)*Record_Size()),SEEK_SET);
    // Read the records from the file
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        fread(buf,Record_Size(),1,input);
        Record_Init(&rec,buf);
        MinHeap_Insert(heap,rec);
    }
    fclose(input);
    int fd = open(argv[5],O_WRONLY);
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        rec = MinHeap_ExtractMin(heap);
        //Record_Print(rec);
        write(fd,rec,Record_Size());
        Record_Destroy(&rec);
    }
    close(fd);
    MinHeap_Destroy(&heap);
    // Send SIGUSR2 to the parent
    kill(getppid(),SIGUSR2);
    return EXIT_SUCCESS;
}
