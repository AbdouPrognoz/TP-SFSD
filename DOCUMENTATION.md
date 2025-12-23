# Sequential File Structures + Hashing Methods
## Practical Work Documentation

---

## Table of Contents
1. [Introduction](#introduction)
2. [File Structure: TnOF](#file-structure-tnof)
3. [Problem Statement](#problem-statement)
4. [Solution: Multi-Pass Partitioning Algorithm](#solution-multi-pass-partitioning-algorithm)
5. [Cost Analysis](#cost-analysis)
6. [Implementation Details](#implementation-details)
7. [Additional Operations](#additional-operations)

---

## Introduction

This document describes the implementation of a **hashing-based partitioning algorithm** for a sequential file structure called **TnOF**. The goal is to efficiently partition a file into K fragments using a hash function, with limited memory (M buffers).

---

## File Structure: TnOF

### What does TnOF stand for?

**TnOF** = **Table non Ordonnée Fixe** (French) = **Unordered Fixed-Size Table**

It is a **sequential file organization** where:
- Records are stored in **fixed-size blocks**
- Records are **NOT ordered** by any key
- Each block can hold a maximum number of records (defined by a loading factor)

### Structure Components

```
┌─────────────────────────────────────────────────────────────┐
│                         FILE (TnOF)                         │
├─────────────────────────────────────────────────────────────┤
│  HEADER                                                     │
│  ┌─────────────┬─────────────┬─────────────────┐            │
│  │  nb_block   │   nb_rec    │  blockCapacity  │            │
│  │  (int)      │   (int)     │     (int)       │            │
│  └─────────────┴─────────────┴─────────────────┘            │
├─────────────────────────────────────────────────────────────┤
│  BLOCK 1                                                    │
│  ┌──────────────────────────────────┬─────────┐             │
│  │  Records[0..blockCapacity-1]     │ nb_rec  │             │
│  └──────────────────────────────────┴─────────┘             │
├─────────────────────────────────────────────────────────────┤
│  BLOCK 2                                                    │
│  ┌──────────────────────────────────┬─────────┐             │
│  │  Records[0..blockCapacity-1]     │ nb_rec  │             │
│  └──────────────────────────────────┴─────────┘             │
├─────────────────────────────────────────────────────────────┤
│  ...                                                        │
├─────────────────────────────────────────────────────────────┤
│  BLOCK N                                                    │
│  ┌──────────────────────────────────┬─────────┐             │
│  │  Records[0..blockCapacity-1]     │ nb_rec  │             │
│  └──────────────────────────────────┴─────────┘             │
└─────────────────────────────────────────────────────────────┘
```

### Key Definitions

| Term | Definition |
|------|------------|
| **Record** | A data unit containing a key (and optionally other fields) |
| **Block** | A fixed-size container that holds multiple records |
| **Header** | Metadata stored at the beginning of the file |
| **MAX_RECORDS** | Maximum physical capacity of a block (e.g., 50) |
| **Loading Factor** | A value between 0 and 1 that determines effective capacity |
| **blockCapacity** | Effective records per block = MAX_RECORDS × Loading Factor |

### Example

If `MAX_RECORDS = 50` and `Loading Factor = 0.8`:
- `blockCapacity = 50 × 0.8 = 40` records per block
- 10 slots are left empty for future insertions

---

## Problem Statement

### Input
- **F**: A TnOF file consisting of **N blocks**
- **b**: Maximum capacity of each block (blockCapacity)
- **K**: Number of fragments to create
- **M**: Number of buffers available in main memory
- **Constraint**: `2 < M < K`

### Hash Function
A uniform hash function **h** with addressable space [0..K-1]:

```
h(e) = e.key mod K
```

For every record `e`, we have: `0 ≤ h(e) ≤ K-1`

### Goal
Partition the file F into **K fragments** (F₀, F₁, ..., F_{K-1}) such that:
- Fragment Fᵢ contains all records where `h(record) = i`
- Use the **most efficient algorithm** given only M buffers

### Challenge
Since **M < K**, we cannot hold all K output buffers in memory simultaneously. We need a strategy to handle this constraint efficiently.

---

## Solution: Multi-Pass Partitioning Algorithm

### Why Multi-Pass?

With only M buffers and K > M fragments:
- We use **1 buffer** to read from the source file
- We use **M-1 buffers** for output fragments
- We can only process **M-1 fragments at a time**
- We need **multiple passes** over the source file

### Number of Passes Required

$$\text{Number of Passes} = \left\lceil \frac{K}{M-1} \right\rceil$$

### Algorithm Pseudocode

```
ALGORITHM MultiPassPartition(sourceFile, K, M)
INPUT:
    sourceFile: TnOF file with N blocks
    K: number of fragments to create
    M: number of available buffers (2 < M < K)
OUTPUT:
    K fragment files (fragment_0, fragment_1, ..., fragment_{K-1})

BEGIN
    // Calculate number of passes needed
    numPasses ← CEILING(K / (M-1))
    
    // Initialize K empty fragment files
    FOR i ← 0 TO K-1 DO
        CreateEmptyTnOF(fragment_i)
    END FOR
    
    // Process fragments in batches of (M-1)
    FOR pass ← 0 TO numPasses-1 DO
        
        // Determine which fragments to handle in this pass
        startFragment ← pass × (M-1)
        endFragment ← MIN(startFragment + (M-1) - 1, K-1)
        
        // Initialize output buffers for this pass
        FOR i ← startFragment TO endFragment DO
            outputBuffer[i - startFragment] ← empty block
        END FOR
        
        // Open source file for reading
        OPEN(sourceFile, 'read')
        
        // Read all blocks from source file
        FOR blockNum ← 1 TO N DO
            inputBuffer ← READBLOCK(sourceFile, blockNum)
            
            // Process each record in the block
            FOR j ← 0 TO inputBuffer.nb_rec - 1 DO
                record ← inputBuffer.T[j]
                hashValue ← record.key MOD K
                
                // Check if this record belongs to current pass
                IF hashValue >= startFragment AND hashValue <= endFragment THEN
                    bufferIndex ← hashValue - startFragment
                    
                    // Add record to appropriate output buffer
                    ADD record TO outputBuffer[bufferIndex]
                    
                    // If buffer is full, write to fragment file
                    IF outputBuffer[bufferIndex] IS FULL THEN
                        WRITEBLOCK(fragment_hashValue, outputBuffer[bufferIndex])
                        outputBuffer[bufferIndex] ← empty block
                    END IF
                END IF
            END FOR
        END FOR
        
        // Flush remaining records in buffers
        FOR i ← startFragment TO endFragment DO
            bufferIndex ← i - startFragment
            IF outputBuffer[bufferIndex] IS NOT EMPTY THEN
                WRITEBLOCK(fragment_i, outputBuffer[bufferIndex])
            END IF
        END FOR
        
        CLOSE(sourceFile)
    END FOR
END
```

### Visual Example

**Given:** N = 10 blocks, K = 7 fragments, M = 3 buffers

Buffer allocation:
- 1 buffer for reading (input)
- M-1 = 2 buffers for writing (output)

Number of passes = ⌈7/2⌉ = **4 passes**

```
┌─────────────────────────────────────────────────────────────┐
│                     PASS 1                                  │
│  Input Buffer ──┬──► Output Buffer 0 ──► Fragment 0        │
│                 └──► Output Buffer 1 ──► Fragment 1        │
│  Processing: h(key) ∈ {0, 1}                                │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                     PASS 2                                  │
│  Input Buffer ──┬──► Output Buffer 0 ──► Fragment 2        │
│                 └──► Output Buffer 1 ──► Fragment 3        │
│  Processing: h(key) ∈ {2, 3}                                │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                     PASS 3                                  │
│  Input Buffer ──┬──► Output Buffer 0 ──► Fragment 4        │
│                 └──► Output Buffer 1 ──► Fragment 5        │
│  Processing: h(key) ∈ {4, 5}                                │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                     PASS 4                                  │
│  Input Buffer ────► Output Buffer 0 ──► Fragment 6         │
│  Processing: h(key) ∈ {6}                                   │
└─────────────────────────────────────────────────────────────┘
```

---

## Cost Analysis

### Definitions
- **Cost** = Total number of disk operations (readBlock + writeBlock)
- **N** = Number of blocks in source file
- **K** = Number of fragments
- **M** = Number of available buffers

### Read Operations

Each pass reads **all N blocks** from the source file:

$$\text{Total Reads} = N \times \left\lceil \frac{K}{M-1} \right\rceil$$

### Write Operations

Every record is written **exactly once** to its destination fragment. The total data volume doesn't change, so:

$$\text{Total Writes} \approx N$$

More precisely: Total writes = sum of blocks across all K fragments

Since records are distributed across K fragments, and total records = N × b:
- Average records per fragment = (N × b) / K
- Blocks per fragment ≈ ⌈N/K⌉
- Total output blocks ≈ K × ⌈N/K⌉ ≈ N

### Total Cost Formula

$$\boxed{\text{Cost} = N \times \left\lceil \frac{K}{M-1} \right\rceil + N = N \times \left( \left\lceil \frac{K}{M-1} \right\rceil + 1 \right)}$$

### Cost Example

**Given:** N = 10, K = 7, M = 3

| Component | Calculation | Result |
|-----------|-------------|--------|
| Passes | ⌈7/(3-1)⌉ = ⌈7/2⌉ | 4 |
| Reads | 10 × 4 | 40 |
| Writes | 10 | 10 |
| **Total Cost** | 40 + 10 | **50** |

### Why is Multi-Pass Optimal?

| Alternative | Reads | Writes | Problem |
|-------------|-------|--------|---------|
| Multi-Pass | N × ⌈K/(M-1)⌉ | N | **Optimal for worst case** |
| Single-Pass Direct | N | N × b | Too many small writes |
| LRU Buffer Swapping | N | Unpredictable | Worst case very bad |
| Sort-Based | 2N | 2N | Only better if K >> M |

**Conclusion:** Multi-Pass guarantees optimal performance for uniformly distributed data.

---

## Implementation Details

### File Structure (C Language)

```c
#define MAX_RECORDS 50

typedef struct Record {
    int key;
} Record;

typedef struct Tblock {
    Record T[MAX_RECORDS];
    int nb_rec;
} Tblock;

typedef struct Header {
    int nb_block;        // Number of blocks in file
    int nb_rec;          // Total number of records
    int blockCapacity;   // Effective capacity (MAX_RECORDS × loadingFactor)
} Header;

typedef struct TnOF {
    FILE *f;
    Header header;
} TnOF;
```

### Abstract Machine Operations

| Function | Description | Cost |
|----------|-------------|------|
| `open()` | Open file and load header | 1 read |
| `close()` | Save header and close file | 1 write |
| `readBlock(file, i, &buf)` | Read block i into buffer | 1 read |
| `writeBlock(file, i, buf)` | Write buffer to block i | 1 write |
| `allocateBlock(file)` | Allocate new block at end | 0 (header update) |
| `getHeader(file, i)` | Get header attribute i | 0 (in memory) |

### Hash Function

```c
int hash(int key, int K) {
    return key % K;
}
```

---

## Additional Operations

### Search in Hashed Structure

To find a record with key `e`:
1. Calculate `i = h(e) = e mod K`
2. Search only in fragment `i` (sequential search)

**Cost:** Reading one fragment instead of entire file

```
ALGORITHM SearchHash(key, K)
    fragmentIndex ← key MOD K
    OPEN(fragment_fragmentIndex)
    Search sequentially in fragment
    CLOSE(fragment)
END
```

### Insert into Hashed Structure

To insert a record with key `e`:
1. Calculate `i = h(e) = e mod K`
2. Insert into fragment `i`

```
ALGORITHM InsertHash(record, K)
    fragmentIndex ← record.key MOD K
    OPEN(fragment_fragmentIndex)
    Insert record (check for duplicates first)
    CLOSE(fragment)
END
```

### Delete from Hashed Structure

To delete a record with key `e`:
1. Calculate `i = h(e) = e mod K`
2. Delete from fragment `i`

```
ALGORITHM DeleteHash(key, K)
    fragmentIndex ← key MOD K
    OPEN(fragment_fragmentIndex)
    Find and delete record
    CLOSE(fragment)
END
```

---

## Project Files

| File | Purpose |
|------|---------|
| `TnOF_BIB.h` | Header file with structure definitions and function declarations |
| `TnOF_BIB.c` | Implementation of abstract machine operations |
| `main.c` | Menu-driven program to test all operations |

---

## Summary

1. **TnOF** is an unordered fixed-size sequential file structure
2. **Problem:** Partition into K fragments using hash function with only M buffers
3. **Solution:** Multi-Pass algorithm processing M-1 fragments per pass
4. **Cost:** N × (⌈K/(M-1)⌉ + 1) disk operations
5. **Optimality:** Guaranteed best performance for worst-case data distribution

---

*Document created for SFSD Practical Work - December 2025*
