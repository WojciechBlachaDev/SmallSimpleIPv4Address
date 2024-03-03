#include <stdio.h>
#include <stdbool.h>
#define MAX_PREFIXES 100

//Struktura danych prefix
struct Prefix {
    unsigned int base;
    char mask;
};
struct Prefix prefixList[MAX_PREFIXES];
int prefixCount = 0;

//Funkcja odczytu adresu IP i sprawdzenia poprawnosci wprowadzanych danych przez uzytkownika
unsigned int read_ip_address() {
    unsigned int octet1, octet2, octet3, octet4;
    printf("Write IP address in the format xxx.xxx.xxx.xxx: ");
    if (scanf("%u.%u.%u.%u", &octet1, &octet2, &octet3, &octet4) != 4) {
        printf("Error: Incorrect IP Address format!\n");
        return 0;
    } else {
        return (octet1 << 24) | (octet2 << 16) | (octet3 << 8) | octet4;
    }
}

//Funkcja odczytu maski i sprawdzenia poprawnosci wprowadzanych danych przez uzytkownika
char read_mask() {
    int mask;
    printf("Write mask of the network (min value = 0, max value = 32): ");
    if (scanf("%d", &mask) != 1 || mask < 0 || mask > 32) {
        printf("Error: Incorrect mask value!\n");
        return -1;
    } else {
        return (char)mask;
    }
}

//Funkcja sprawdzenia czy adres IP miesci sie w danym prefixie
bool is_subset(unsigned int ip, unsigned int base, char mask) {
    unsigned int masked_ip = ip & ((0xFFFFFFFF) << (32 - mask));
    return masked_ip == base;
}

//FUnkcja dodawania nowych prefixow
void add(unsigned int base, char mask) {
    for (int i = 0; i < prefixCount; i++) {
        if (prefixList[i].base == base && prefixList[i].mask == mask) {
            printf("Prefix already exists in the list.\n");
            return;
        }
    }
    if (prefixCount >= MAX_PREFIXES) {
        printf("Error: Prefix list is full!\n");
        return;
    }
    prefixList[prefixCount].base = base;
    prefixList[prefixCount].mask = mask;
    prefixCount++;
    printf("Prefix added successfully: %u.%u.%u.%u/%d\n", (base >> 24) & 0xFF, (base >> 16) & 0xFF, (base >> 8) & 0xFF, base & 0xFF, mask);
}

//Funkcja usuwania prefixow
void del() {
    if (prefixCount == 0) {
        printf("Prefix list is empty.\n");
        return;
    }
    printf("Prefixes in the list:\n");
    for (int i = 0; i < prefixCount; i++) {
        printf("%d. %u.%u.%u.%u/%d\n", i + 1, (prefixList[i].base >> 24) & 0xFF, (prefixList[i].base >> 16) & 0xFF, (prefixList[i].base >> 8) & 0xFF, prefixList[i].base & 0xFF, prefixList[i].mask);
    }
    int choice;
    printf("Select the prefix number to delete (1 - %d): ", prefixCount);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > prefixCount) {
        printf("Invalid choice.\n");
        return;
    }
    unsigned int base = prefixList[choice - 1].base;
    char mask = prefixList[choice - 1].mask;
    for (int i = choice - 1; i < prefixCount - 1; i++) {
        prefixList[i] = prefixList[i + 1];
    }
    prefixCount--;
    printf("Prefix deleted successfully: %u.%u.%u.%u/%d\n", (base >> 24) & 0xFF, (base >> 16) & 0xFF, (base >> 8) & 0xFF, base & 0xFF, mask);
}

//Funckja sprawdzajaca czy adres IP znajduje sie w zapisanych prefixach
void check(unsigned int ip) {
    char highest_mask = -1;
    bool found = false;
    for (int i = 0; i < prefixCount; i++) {
        if (is_subset(ip, prefixList[i].base, prefixList[i].mask)) {
            if (!found || prefixList[i].mask > highest_mask) {
                found = true;
                highest_mask = prefixList[i].mask;
            }
        }
    }
    if (!found) {
        printf("IP address not found in the prefix list.\n");
    } else {
        printf("Smallest mask in the prefix list containing the IP address: %u.%u.%u.%u/%d\n", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF, highest_mask);
    }
}

//Funkcja zapisu do pliku .txt dodawanych prefixow
void save() {
    FILE *file = fopen("prefixes.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    for (int i = 0; i < prefixCount; i++) {
        fprintf(file, "%u.%u.%u.%u/%d\n", (prefixList[i].base >> 24) & 0xFF, (prefixList[i].base >> 16) & 0xFF, (prefixList[i].base >> 8) & 0xFF, prefixList[i].base & 0xFF, prefixList[i].mask);
    }
    fclose(file);
    printf("Prefix list saved to file: prefixes.txt\n");
}

//Funkcja ladowania zapisanych w pliku .txt prefixow
void load() {
    FILE *file = fopen("prefixes.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }
    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        unsigned int octet1, octet2, octet3, octet4;
        int mask;
        if (sscanf(line, "%u.%u.%u.%u/%d", &octet1, &octet2, &octet3, &octet4, &mask) == 5) {
            unsigned int base = (octet1 << 24) | (octet2 << 16) | (octet3 << 8) | octet4;
            if (prefixCount < MAX_PREFIXES) {
                prefixList[prefixCount].base = base;
                prefixList[prefixCount].mask = (char) mask;
                prefixCount++;
            } else {
                printf("Error: Prefix list is full!\n");
                break;
            }
        } else {
            printf("Error while loading prefixes from file.\n");
            break;
        }
    }
    fclose(file);
    printf("Prefix list loaded from file: prefixes.txt\n");
}

//Funckja pozwalajaca na wyczyszczenie zapisanych w pliku .txt prefixow
void clear_file_content() {
    FILE *file = fopen("prefixes.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    fclose(file);
    printf("File content cleared successfully.\n");
}

//Funkcja pokazujaca liste zapisanych w programie prefixow
void show_prefixes() {
    printf("Prefix list:\n");
    for (int i = 0; i < prefixCount; i++) {
        printf("%d. %u.%u.%u.%u/%d\n", i + 1, (prefixList[i].base >> 24) & 0xFF, (prefixList[i].base >> 16) & 0xFF, (prefixList[i].base >> 8) & 0xFF, prefixList[i].base & 0xFF, prefixList[i].mask);
    }
    printf("\nPress Enter to return to the menu...");
    while (getchar() != '\n');
    getchar();
}



int main() {
    load();
    char menuChoice;
    printf("PROGRAM LOADED\n\n");
    while (true) {
        printf("Select program option:\n");
        printf("1. Add new IPv4 prefix to list\n");
        printf("2. Check IPv4 prefix on the list\n");
        printf("3. Delete IPv4 prefix from list\n");
        printf("4. Save prefix list to file\n");
        printf("5. Clear file content\n");
        printf("6. Show added prefixes\n");
        printf("7. Exit the program\n");
        printf("Choice: ");
        scanf(" %c", &menuChoice);
        switch (menuChoice) {
            case '1': {
                unsigned int ip = read_ip_address();
                char mask = read_mask();
                if (mask >= 0) {
                    add(ip, mask);
                }
                break;
            }
            case '2': {
                unsigned int ip = read_ip_address();
                check(ip);
                break;
            }
            case '3': {
                del();
                break;
            }
            case '4': {
                save();
                break;
            }
            case '5': {
                clear_file_content();
                break;
            }
            case '6': {
                show_prefixes();
                break;
            }
            case '7': {
                char confirm;
                do {
                    printf("Are you sure you want to exit the program? All unsaved data will be lost. (y/n): ");
                    scanf(" %c", &confirm);
                    if (confirm == 'y' || confirm == 'Y') {
                        printf("Exiting the program.\n");
                        return 0;
                    } else if (confirm == 'n' || confirm == 'N') {
                        break;
                    } else {
                        printf("Invalid choice.\n Please enter 'y' to exit or 'n' to return to the menu!\n");
                    }
                } while (true);
                break;
            }
            default:
                printf("Invalid choice. Please choose again.\n");
        }
    }
}
