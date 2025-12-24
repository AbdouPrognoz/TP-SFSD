#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TnOF_BIB.h"

void printMenu() {
    printf("\n================== TnOF MENU ==================\n");
    printf("1. Create a new file (Initial Load)\n");
    printf("2. Display file contents\n");
    printf("3. Search for a record\n");
    printf("4. Insert a record\n");
    printf("5. Delete a record (Physical)\n");
    printf("6. Partition file (Hash into K fragments)\n");
    printf("7. Display a single partition\n");
    printf("8. Display all partitions\n");
    printf("9. Search in the partitioned file\n");
    printf("10. Insert into the partitioned file\n");
    printf("11. Delete from the partitioned file\n");
    printf("12. Exit\n");
    printf("================================================\n");
    printf("Enter your choice: ");
}

int main()
{
    TnOF file;
    char file_name[50];
    int choice, key, found, i, j;
    float loadingFactor;
    Record rec;

    printf("Enter the file name to work with: ");
    scanf("%s", file_name);
    getchar();

    do {
        printMenu();
        scanf("%d", &choice);
        getchar();

        switch(choice) {
            case 1: // Initial Load
                printf("\n--- CREATE NEW FILE ---\n");
                initialLoad(&file);
                printf("File created successfully!\n");
                break;

            case 2: // Display
                printf("\n--- DISPLAY FILE CONTENTS ---\n");
                displayTnOF(file_name);
                break;

            case 3: // Search
                printf("\n--- SEARCH FOR A RECORD ---\n");
                printf("Enter key to search: ");
                scanf("%d", &key);
                getchar();
                
                searchTnOF(key, file_name, &found, &i, &j);
                
                if (found)
                    printf("Record with key %d found at block %d, position %d\n", key, i, j);
                else
                    printf("Record with key %d not found. Can be inserted at block %d, position %d\n", key, i, j);
                break;

            case 4: // Insert
                printf("\n--- INSERT A RECORD ---\n");
                printf("Enter key to insert: ");
                scanf("%d", &(rec.key));
                getchar();
                
                inserTnOF(file_name, rec);
                break;

            case 5: // Delete
                printf("\n--- DELETE A RECORD ---\n");
                printf("Enter key to delete: ");
                scanf("%d", &key);
                getchar();
                
                deleteTnOFphy(file_name, key);
                break;

            case 6: // Partition
                printf("\n--- PARTITION FILE ---\n");
                printf("Will partition file: %s\n", file_name);
                int K, M;
                printf("Enter K (number of fragments, must be >= 4): ");
                scanf("%d", &K);
                getchar();
                
                if (K < 4) {
                    printf("Invalid K! Must be >= 4 (so valid M can exist)\n");
                    break;
                }
                
                printf("Enter M (number of buffers, must be > 2 and < %d): ", K);
                scanf("%d", &M);
                getchar();
                
                if (M <= 2 || M >= K) {
                    printf("Invalid M! Must be 2 < M < K (so M can be 3 to %d)\n", K-1);
                } else {
                    partition(file_name, K, M);
                }
                break;

            case 7: // Display single partition
                printf("\n--- DISPLAY SINGLE PARTITION ---\n");
                int partNum;
                printf("Enter partition number (0 to K-1): ");
                scanf("%d", &partNum);
                getchar();
                
                char partName[30];
                sprintf(partName, "partition%d", partNum);
                displayTnOF(partName);
                break;

            case 8: // Display all partitions
                printf("\n--- DISPLAY ALL PARTITIONS ---\n");
                printf("Enter K (number of partitions): ");
                scanf("%d", &K);
                getchar();
                
                for (int p = 0; p < K; p++) {
                    printf("\n========== PARTITION %d ==========\n", p);
                    char pName[30];
                    sprintf(pName, "partition%d", p);
                    displayTnOF(pName);
                }
                break;

            case 9: // Search in the partitioned file
                printf("\n--- SEARCH IN PARTITIONED FILE ---\n");
                printf("Enter K (number of partitions used): ");
                scanf("%d", &K);
                getchar();
                
                printf("Enter key to search: ");
                scanf("%d", &key);
                getchar();
                
                searchPartitioned(key, K, &found, &i, &j);
                
                if (found)
                    printf("Record with key %d found at block %d, position %d\n", key, i, j);
                else
                    printf("Record with key %d not found. Can be inserted at block %d, position %d\n", key, i, j);
                break;

            case 10: // Insert into the partitioned file
                printf("\n--- INSERT INTO PARTITIONED FILE ---\n");
                printf("Enter K (number of partitions): ");
                scanf("%d", &K);
                getchar();
                
                printf("Enter key to insert: ");
                scanf("%d", &(rec.key));
                getchar();
                
                insertPartitioned(rec, K);
                break;

            case 11: // Delete from the partitioned file
                printf("\n--- DELETE FROM PARTITIONED FILE ---\n");
                printf("Enter K (number of partitions): ");
                scanf("%d", &K);
                getchar();
                
                printf("Enter key to delete: ");
                scanf("%d", &key);
                getchar();
                
                deletePartitioned(key, K);
                break;    

            case 12: // Exit
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid choice! Please try again.\n");
        }

    } while(choice != 12);

    return 0;
}