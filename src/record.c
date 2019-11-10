#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/record.h"

// Define record structure
struct record
{
    long registrationNumber;
    char firstname[20];
    char lastname[20];
    char street[20];
    int streetNum;
    char city[20];
    char zip[6];
    float salary;
};

int Record_Size() {
    return sizeof(struct record);
}

int Record_Init(Record *rec,char *bytes) {
    // Allocate memory for the record
    if (((*rec) = malloc(sizeof(struct record))) == NULL) {
        printf("Not enough memory for record\n");
        return 0;
    }
    // Copy bytes to the record structure
    memcpy(*rec,bytes,sizeof(struct record));
    return 1;
}

int Record_Compare(Record r1,Record r2,int field) {
    // Return 1 if r1[field] < r2[field] and 0 otherwise
    switch (field)
    {
        case 1: return r1->registrationNumber < r2->registrationNumber;
        case 2: return strcmp(r1->firstname,r2->firstname) < 0;
        case 3: return strcmp(r1->lastname,r2->lastname) < 0;
        case 4: return strcmp(r1->street,r2->street) < 0;
        case 5: return r1->streetNum < r2->streetNum;
        case 6: return strcmp(r1->city,r2->city) < 0;
        case 7: return strcmp(r1->zip,r2->zip) < 0;
        case 8: return r1->salary < r2->salary;
        default: return r1->registrationNumber < r2->registrationNumber;
    }
}

void Record_Print(Record rec) {
    if (rec != NULL)
        printf("%ld %s %s %s %d %s %s %f\n",rec->registrationNumber,rec->firstname,rec->lastname,rec->street,rec->streetNum,rec->city,rec->zip,rec->salary);
}

int Record_Destroy(Record *rec) {
    // Free all allocated memory
    if (*rec != NULL) {
        free(*rec);
        *rec = NULL;
        return 1;
    }
    return 0;
}
