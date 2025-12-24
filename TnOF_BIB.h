#ifndef _BIBLIO_TOF_H
#define _BIBLIO_TOF_H
#include <stdio.h>

#define MAX_RECORDS 50


typedef struct Record
{
    int key;
}Record;

typedef struct Tblock
{
    Record T[MAX_RECORDS];
    int nb_rec;
}Tblock;

typedef struct Header
{
    int nb_block;
    int nb_rec;
    int blockCapacity;   
}Header;

typedef struct TnOF
{
    FILE *f;
    Header header;
}TnOF;

// classic functions
void open(TnOF *file, const char *filename, const char mode);

void close(TnOF file); 

int getHeader(TnOF file, int i); 

int readBlock(TnOF file, int i, Tblock *buf); 

int writeBlock(TnOF file, int i, Tblock buf); 

void allocateBlock(TnOF *file); 


// classic tnof funcitons
void initialLoad(TnOF *file); 

void displayTnOF(const char *file_name); 

void searchTnOF(const int key, const char *filename, int *found, int *i, int *j); 

void inserTnOF(const char *filename, Record record); 

void deleteTnOFphy(const char *filename, int key); 

// TP funcitons

int hash(int key , int k); // Hash function

void partition(const char *sourceFile , int k , int M) ; // the main partitioning function  // multi pass solution

void searchPartitioned(const int key, int K, int *found, int *i, int *j); // search for a record within the new structure

void insertPartitioned(Record record, int K); //insert a record into the new structure

void deletePartitioned(int key, int K); //delete a record from the new structure

#endif 