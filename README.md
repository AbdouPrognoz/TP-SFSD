# TnOF - Table non Ordonnée Fixe (Unordered Fixed-Size Table)

A C implementation of an abstract machine for managing unordered fixed-size file structures with hash-based partitioning capabilities.

## 📋 Overview

This project implements a **TnOF (Table non Ordonnée Fixe)** file structure, which is an unordered table with fixed-size blocks. The implementation includes:

- Basic file operations (create, read, write, search, insert, delete)
- **Loading factor** concept for flexible block capacity
- **Multi-pass hash partitioning algorithm** to split files into K fragments using M buffers

## 🎯 Features

### Core Operations

- **Create**: Initialize a new TnOF file with customizable loading factor
- **Display**: View all records in the file with block-by-block details
- **Search**: Find a record by key with O(n) sequential search
- **Insert**: Add new records (duplicates allowed)
- **Delete**: Physical deletion with last-record replacement strategy

### Hash Partitioning (TP Assignment)

- Partition a TnOF file into **K fragments** using a hash function `h(key) = key % K`
- Works with limited memory: only **M buffers** available (where 2 < M < K)
- Implements **multi-pass algorithm** when M-1 < K
- Operations on partitioned structure: search, insert, delete

## 🏗️ Project Structure

```
TnOF/
├── main.c          # Menu-driven test program
├── TnOF_BIB.c      # Implementation of all functions
├── TnOF_BIB.h      # Header file with structures and prototypes
├── DOCUMENTATION.md # Detailed algorithm documentation
└── README.md       # This file
```

## 📊 Data Structures

### Record

```c
typedef struct Record {
    int key;
} Record;
```

### Block (Tblock)

```c
typedef struct Tblock {
    Record T[MAX_RECORDS];  // Array of records (MAX = 50)
    int nb_rec;             // Number of records in this block
} Tblock;
```

### Header

```c
typedef struct Header {
    int nb_block;      // Total number of allocated blocks
    int nb_rec;        // Total number of records
    int blockCapacity; // Effective capacity based on loading factor
} Header;
```

## 🔧 Compilation & Execution

### Compile

```bash
gcc -o TnOF main.c TnOF_BIB.c
```

### Run

```bash
./TnOF
```

## 📖 Usage

### Menu Options

```
================== TnOF MENU ==================
1. Create a new file (Initial Load)
2. Display file contents
3. Search for a record
4. Insert a record
5. Delete a record (Physical)
6. Partition file (Hash into K fragments)
7. Display a single partition
8. Display all partitions
9. Search in the partitioned file
10. Insert into the partitioned file
11. Delete from the partitioned file
12. Exit
================================================
```

### Example: Create and Partition a File

1. **Create a file** with 20 records:

   - Choose option `1`
   - Enter filename: `test`
   - Enter loading factor: `0.2` (gives 10 records per block)
   - Enter number of blocks: `2`
   - Enter 20 keys (e.g., 0-19)

2. **Partition into 5 fragments**:

   - Choose option `6`
   - Enter K = `5` (number of fragments)
   - Enter M = `3` (number of buffers)
   - Files `partition0` through `partition4` will be created

3. **Verify partitions**:
   - Choose option `8` to display all partitions
   - Records are distributed by `key % K`

## 🧮 Multi-Pass Partitioning Algorithm

When M-1 < K (not enough buffers for all fragments), the algorithm uses multiple passes:

### Cost Formula

```
Total disk operations = N × (⌈K/(M-1)⌉ + 1)
```

Where N = number of source blocks

### Algorithm Steps

1. Calculate passes needed: `passes = ⌈K/(M-1)⌉`
2. For each pass:
   - Process M-1 fragments (one buffer reserved for input)
   - Read all source blocks
   - Hash each record and buffer if it belongs to current pass
   - Write full buffers to fragment files
3. Flush remaining buffers after each pass

### Example

- K = 5, M = 3 → 3 passes needed
- Pass 1: fragments 0, 1
- Pass 2: fragments 2, 3
- Pass 3: fragment 4

## 📝 Loading Factor

The loading factor (0.0 to 1.0) determines the effective block capacity:

```
blockCapacity = MAX_RECORDS × loadingFactor
```

- `MAX_RECORDS = 50` (maximum physical capacity)
- `loadingFactor = 0.8` → 40 records per block
- `loadingFactor = 0.2` → 10 records per block

This allows flexibility in testing and demonstrates realistic file behavior.

## 🎓 Academic Context

This project is a practical work (TP) assignment for **SFSD (Structures de Fichiers et Structures de Données)** course, focusing on:

- Abstract machine concept for file structures
- Hash-based file partitioning
- Memory-constrained algorithms (multi-pass processing)
- Cost analysis of disk operations

## 📄 License

This project is for educational purposes.

## 👤 Author

Developed as part of SFSD coursework.
