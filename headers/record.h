#ifndef RECORD_H
#define RECORD_H

typedef struct record *Record;

int Record_Size();
int Record_Init(Record*,char*);
int Record_Compare(Record,Record,int);
void Record_Print(Record);
int Record_Destroy(Record*);

#endif