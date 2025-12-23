#include <stdio.h>
#include <stdlib.h>
#include "TnOF_BIB.h"

void open(TnOF *file, const char *filename, const char mode) //--- Opening a file ---//
{
    if (mode == 'o') //--- Open in old mode ---//
    {
        file->f = fopen(filename, "rb+");
        fread(&(file->header), sizeof(Header), 1, file->f);
    }
    else //--- Open in new mode with header initialization ---//
    {
        file->f = fopen(filename, "wb+");
        file->header.nb_block = 0;
        file->header.nb_rec = 0;
        fwrite(&(file->header), sizeof(Header), 1, file->f);
    }
}

void close(TnOF file) //--- Close a file and save the header ---//
{
    rewind(file.f);
    fwrite(&(file.header), sizeof(Header), 1, file.f);
    fclose(file.f);
    file.f = NULL;
}

int readBlock(TnOF file, int i, Tblock *buf) //--- Read the block at position i from a file into a buffer ---//
{
    if ((i > file.header.nb_block) || (i < 1)) return 0; //--- Stop reading if the block does not exist ---//
    fseek(file.f, sizeof(Header) + (i - 1) * sizeof(Tblock), 0);
    fread(buf, sizeof(Tblock), 1, file.f);
    return 1;
}

int writeBlock(TnOF file, int i, Tblock buf) //--- Write a block to the file at the ith position ---//
{
    if ((i > file.header.nb_block) || (i < 1)) return 0; //--- Stop writing if the position does not exist ---//
    fseek(file.f, sizeof(Header) + (i - 1) * sizeof(Tblock), 0);
    fwrite(&buf, sizeof(Tblock), 1, file.f);
    return 0;
}

int getHeader(TnOF file, int i) //--- Return the value of the ith header attribute ---//
{                           
    switch(i)
    {
        case 1:
            return file.header.nb_block; //--- 1st attribute ---//
        case 2:
            return file.header.nb_rec; //--- 2nd attribute ---//
        case 3:
            return file.header.blockCapacity; //--- 3rd attribute: block capacity ---//
        default:
            return 0;
    }
}

void allocateBlock(TnOF *file) //--- Allocate a new block in the file ---//
{
    Tblock newBlock;
    newBlock.nb_rec = 0;
    fseek(file->f, 0, SEEK_END);
    fwrite(&newBlock, sizeof(Tblock), 1, file->f);
    file->header.nb_block++;
}

void initialLoad(TnOF *file) //--- Create a new file and initialize it ---//
{
    char name[20];
    printf("Enter the name of the file to create: ");
    scanf("%s", name);
    getchar();
    open(file, name, 'n');

    // Display max records info and get loading factor
    printf("Maximum records per block: %d\n", MAX_RECORDS);
    printf("Enter loading factor (0.0 to 1.0): ");
    float loadingFactor;
    scanf("%f", &loadingFactor);
    getchar();
    
    if (loadingFactor <= 0.0 || loadingFactor > 1.0) {
        printf("Invalid loading factor! Using default 0.8\n");
        loadingFactor = 0.8;
    }

    // Calculate effective block capacity based on loading factor
    int blockCapacity = (int)(MAX_RECORDS * loadingFactor);
    if (blockCapacity < 1) blockCapacity = 1; // At least 1 record per block
    file->header.blockCapacity = blockCapacity; // Store in header
    printf("Effective records per block: %d\n", blockCapacity);

    int numBlocks, i = 0, j = 0;
    printf("Enter the number of blocks to create: ");
    do
    {
        scanf("%d", &numBlocks);
        getchar();
    } while(numBlocks < 1);

    int numRecords = numBlocks * blockCapacity; // Total records = blocks × capacity

    Tblock buf;
    buf.nb_rec = 0;
    printf("\nProceed with data entry (%d records total):\n", numRecords);
    while(i < numRecords)
    {
        if((j + 1) <= blockCapacity) //--- Use loading factor capacity instead of MAX_RECORDS ---//
        {
            printf("\nKey %d = ", i + 1);
            scanf("%d", &(buf.T[j].key));
            getchar();
            i++;
            j++;
        }
        else
        {
            allocateBlock(file);
            buf.nb_rec = j;
            writeBlock(*file, file->header.nb_block, buf);
            j = 0;
        }
    }
    if(j != 0)
    {
        allocateBlock(file);
        buf.nb_rec = j;
        writeBlock(*file, file->header.nb_block, buf);
    }
    file->header.nb_rec = numRecords;
    close(*file);
}


void searchTnOF(const int key, const char *filename,int *found, int *i, int *j)
{
    int stop = 0;
    TnOF file;
    Tblock buffer;
    open(&file, filename, 'o');
    int nbBlocks = getHeader(file, 1);
    *i=0, *j=0,*found=0;
    while((*i<nbBlocks) && (!*found) && (!stop))
    {
        *i=*i+1;

        readBlock(file, *i, &buffer);
        *j=0;
        while(*j<buffer.nb_rec){
            if(key == buffer.T[*j].key){
                *found = 1;
                stop=1;
                break;
            }

            *j=*j+1;
        }
    }
    //in case the the record should be in a new bloc
    int blockCapacity = getHeader(file, 3);
    if(*j >= blockCapacity){
        *i=*i+1;
        *j=0;
    } 

    close (file);
}


void inserTnOF(const char *filename, Record record) //--- Procedure to insert a record ---//
{
    int found, i, j;
    TnOF file;
    Tblock buffer;

    searchTnOF(record.key, filename,&found, &i, &j); //--- Searching for the record in the file ---//

    if (found==0)
    {
        open(&file, filename, 'o');

        if(i>getHeader(file,1)) allocateBlock(&file); //if the last block is full allocate a new one

        readBlock(file, i, &buffer);
        buffer.T[j]=record;
        buffer.nb_rec++;
        writeBlock(file,i,buffer);
        file.header.nb_rec++;

        close(file);
        printf("Your record has been added successfully\n");
    }
    else printf("Your record already exists in the file\n");
}


void deleteTnOFphy(const char *filename, int key) //--- Physical deletion procedure ---//
{
    int i, j,found;
    searchTnOF(key, filename,&found, &i, &j);
    printf("%d %d\n",i,j);

    if (found)
    {
        TnOF file;
        Tblock buffer;
        Record temp;
        open(&file, filename, 'o');

        readBlock(file, getHeader(file,1), &buffer);
        temp = buffer.T[buffer.nb_rec-1]; //get the last record
        buffer.nb_rec--;
        file.header.nb_rec = file.header.nb_rec - 1;

        if (buffer.nb_rec == 0) //--- If a block becomes empty ---//
        {
            file.header.nb_block = file.header.nb_block - 1;
        }else{
            writeBlock(file, getHeader(file,1), buffer);
        }

        readBlock(file, i, &buffer);
        buffer.T[j]=temp; //replace the record you want to delete with the last record
        writeBlock(file, i, buffer);

        close(file);

        printf("Your record has been deleted successfully\n");
    }
    else printf("Your record doesn't exist in the file\n");
}



void displayTnOF(const char *filename){
    TnOF file;
    Tblock buffer;

    open(&file, filename, 'o');
    int i=1,j,nb_blocks=getHeader(file, 1);


    printf("Displaying header: \n");
    printf("\t- Number of allocated blocks: %d\n\t- Total number of records: %d\n", nb_blocks, getHeader(file, 2));


    while(i<=nb_blocks){
        readBlock(file, i, &buffer);
        printf("Displaying block: %d  nb_rec=%d\n", i,buffer.nb_rec);

        j = 0;
        while (j < buffer.nb_rec)
        {
            printf("\t- Key: %d\n", buffer.T[j].key); //--- Only display key (no info field) ---//
            j++;
        }
        i++;
    }
    close(file);
}


int hash(int key , int k)
{
return (key % k);
}


void partition(const char *sourceFile, int K, int M) {
    // Step 1: Calculate number of passes needed
    int passes = (K + M - 2) / (M - 1);  // ceiling of K/(M-1)
    printf("Partitioning into %d fragments using %d buffers\n", K, M);
    printf("Number of passes required: %d\n", passes);

    // Step 2: Get blockCapacity and the nbBlocks from source file
    TnOF srcFile;
    open(&srcFile, sourceFile, 'o');
    if (srcFile.f == NULL) {
        printf("Error: Could not open source file '%s'\n", sourceFile);
        return;
    }
    int blockCapacity = getHeader(srcFile, 3);
    int nbBlocks = getHeader(srcFile, 1);
    printf("Source file: %d blocks, blockCapacity=%d\n", nbBlocks, blockCapacity);
    close(srcFile);
    
    if (nbBlocks == 0) {
        printf("Error: Source file is empty!\n");
        return;
    }    

    // Step 3: Create K empty fragment files with same blockCapacity
    char filename[30];
    for (int i = 0; i < K; i++) {
        sprintf(filename, "partition%d", i);
        TnOF fragFile;
        open(&fragFile, filename, 'n');
        fragFile.header.blockCapacity = blockCapacity;  // we work as if the fragmented file has the Same capacity as source
        close(fragFile);
    }

    // Step 4: Multi-pass algorithm
    for (int pass = 0; pass < passes; pass++) {
        // 4a: Calculate fragment range for this pass
        int startFragment = pass * (M - 1);
        int endFragment = startFragment + (M - 2);
        if (endFragment >= K) endFragment = K - 1;
        
        int numBuffers = endFragment - startFragment + 1;  // actual number of fragments in this pass
        printf("\nPass %d: Processing fragments %d to %d (%d buffers)\n", pass + 1, startFragment, endFragment, numBuffers);

        // 4b: Initialize output buffers for this pass
        Tblock outputBuffers[numBuffers];
        for (int i = 0; i < numBuffers; i++) {
            outputBuffers[i].nb_rec = 0;
        }

        // 4c: Open source file and read all blocks
        open(&srcFile, sourceFile, 'o');
        
        for (int blockNum = 1; blockNum <= nbBlocks; blockNum++) {
            Tblock inputBuffer;
            readBlock(srcFile, blockNum, &inputBuffer);

            // 4d: Process each record in the block
            for (int j = 0; j < inputBuffer.nb_rec; j++) {
                Record rec = inputBuffer.T[j];
                int hashValue = hash(rec.key, K);

                // Check if this record belongs to current pass
                if (hashValue >= startFragment && hashValue <= endFragment) {
                    int bufferIndex = hashValue - startFragment;

                    // Add record to the appropriate output buffer
                    outputBuffers[bufferIndex].T[outputBuffers[bufferIndex].nb_rec] = rec;
                    outputBuffers[bufferIndex].nb_rec++;

                    // If buffer is full, write to fragment file
                    if (outputBuffers[bufferIndex].nb_rec >= blockCapacity) {
                        sprintf(filename, "partition%d", hashValue);
                        TnOF fragFile;
                        open(&fragFile, filename, 'o');
                        allocateBlock(&fragFile);
                        writeBlock(fragFile, fragFile.header.nb_block, outputBuffers[bufferIndex]);
                        fragFile.header.nb_rec += outputBuffers[bufferIndex].nb_rec;
                        close(fragFile);

                        // Reset buffer
                        outputBuffers[bufferIndex].nb_rec = 0;
                    }
                }
            }
        }
        
        close(srcFile);

        // 4e: Flush remaining non-empty buffers
        for (int i = 0; i < numBuffers; i++) {
            if (outputBuffers[i].nb_rec > 0) {
                int fragIndex = startFragment + i;
                sprintf(filename, "partition%d", fragIndex);
                TnOF fragFile;
                open(&fragFile, filename, 'o');
                allocateBlock(&fragFile);
                writeBlock(fragFile, fragFile.header.nb_block, outputBuffers[i]);
                fragFile.header.nb_rec += outputBuffers[i].nb_rec;
                close(fragFile);
            }
        }
    }

    printf("\nPartitioning complete! Created %d fragment files.\n", K);
}


