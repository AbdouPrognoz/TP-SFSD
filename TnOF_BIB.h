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
    int blockCapacity;    // Effective records per block (MAX_RECORDS * loadingFactor)
}Header;

typedef struct TnOF
{
    FILE *f;
    Header header;
}TnOF;


void open(TnOF *file, const char *filename, const char mode); // Open a file

void close(TnOF file); // Close a file

int getHeader(TnOF file, int i); // Get header information

int readBlock(TnOF file, int i, Tblock *buf); // Read block

int writeBlock(TnOF file, int i, Tblock buf); // Write block


void allocateBlock(TnOF *file); // Allocate a new block

void initialLoad(TnOF *file); // Initialize the file with loading factor (prompted inside)

void displayTnOF(const char *file_name); // Display the TOF

void searchTnOF(const int key, const char *filename, int *found, int *i, int *j); // Search in the TOF

void inserTnOF(const char *filename, Record record); // Insert a record

void deleteTnOFphy(const char *filename, int key); // Physical deletion

int hash(int key , int k); // Hash function

void partition(const char *sourceFile , int k , int M) ; // the main partitioning function  // multi pass solution


#endif // _BIBLIO_TOF_H
