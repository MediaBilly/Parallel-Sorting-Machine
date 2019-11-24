#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/times.h>
#include "../headers/record.h"
#include "../headers/utilities.h"

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
    // Usage: ./sorter_quicksort <inputfile> <first_record> <last_record> <column> <output_file>
    double t1, t2;
    struct tms tb1, tb2;
    double ticspersec;
    ticspersec = (double)sysconf(_SC_CLK_TCK);
    t1 = (double)times(&tb1);
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
    // Sort the records
    quicksort(records,0,lastRecord - firstRecord,field);
    // Send the sorted records to the output file(named pipe)
    int recordsPerBuf = PIPE_SIZE/Record_Size();
    char recBuf[PIPE_SIZE];
    int fd = open(argv[5],O_WRONLY);
    int n = 0;
    for (i = 0;i <= lastRecord - firstRecord;i++) {
        if (n == recordsPerBuf) {
            write(fd,recBuf,n * Record_Size());
            n = 0;
        }
        memcpy(recBuf + n*Record_Size(),records[i],Record_Size());
        n++;
        Record_Destroy(records + i);
    }
    if (n > 0)
        write(fd,recBuf,n * Record_Size());
    // Send sorting time
    t2 = (double)times(&tb2);
    double sortTime = (t2 - t1)/ticspersec;
    char sTime[sizeof(double)];
    //sprintf(sTime,"%.2lf",sortTime);
    memcpy(sTime,&sortTime,sizeof(double));
    write(fd,sTime,sizeof(double));
    close(fd);
    free(records);
    // Send SIGUSR2 to the parent
    kill(getppid(),SIGUSR2);
    return EXIT_SUCCESS;
}
