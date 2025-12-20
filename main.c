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
    printf("6. Exit\n");
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
                printf("Enter loading factor (0.0 to 1.0): ");
                scanf("%f", &loadingFactor);
                getchar();
                
                if (loadingFactor <= 0.0 || loadingFactor > 1.0) {
                    printf("Invalid loading factor! Using default 0.8\n");
                    loadingFactor = 0.8;
                }
                
                initialLoad(&file, loadingFactor);
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

            case 6: // Exit
                printf("Exiting program. Goodbye!\n");
                break;

            default:
                printf("Invalid choice! Please try again.\n");
        }

    } while(choice != 6);

    return 0;
}

