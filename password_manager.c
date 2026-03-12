/*
 * ADVANCED PASSWORD MANAGER v2.0
 * Language: C
 * Submitted as Additional Assignment
 * Student Name: Dipin Kakkar
 * Course: C Programming
 * Date: March 2026
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define FILE_NAME "passwords.txt"
#define MAX_ACCOUNTS 100
#define MAX_LENGTH 100
#define PASS_LENGTH 16

// ==================== STRUCTURE ====================
typedef struct {
    char account[MAX_LENGTH];
    char password[MAX_LENGTH];
    char strength[10];
    char created[20];
} Account;

typedef struct {
    char master_hash[65];     // Simple hash simulation
    Account accounts[MAX_ACCOUNTS];
    int count;
} PasswordData;

// ==================== HELPER FUNCTIONS ====================
void simple_hash(const char *str, char *hash) {
    unsigned long h = 5381;
    int c;
    while ((c = *str++)) {
        h = ((h << 5) + h) + c;  // djb2 hash
    }
    sprintf(hash, "%lu", h);
}

void load_data(PasswordData *data) {
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) {
        strcpy(data->master_hash, "");
        data->count = 0;
        return;
    }
    fscanf(f, "%s", data->master_hash);
    fscanf(f, "%d", &data->count);
    for (int i = 0; i < data->count; i++) {
        fscanf(f, "%s %s %s %s", 
               data->accounts[i].account,
               data->accounts[i].password,
               data->accounts[i].strength,
               data->accounts[i].created);
    }
    fclose(f);
}

void save_data(PasswordData *data) {
    FILE *f = fopen(FILE_NAME, "w");
    fprintf(f, "%s\n", data->master_hash);
    fprintf(f, "%d\n", data->count);
    for (int i = 0; i < data->count; i++) {
        fprintf(f, "%s %s %s %s\n",
                data->accounts[i].account,
                data->accounts[i].password,
                data->accounts[i].strength,
                data->accounts[i].created);
    }
    fclose(f);
}

void generate_password(char *password, int length) {
    const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=[]{}|;:,.<>?";
    int n = strlen(chars);
    srand(time(0));
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % n];
    }
    password[length] = '\0';
}

char* check_strength(const char *password) {
    int score = 0;
    if (strlen(password) >= 12) score++;
    if (strpbrk(password, "ABCDEFGHIJKLMNOPQRSTUVWXYZ")) score++;
    if (strpbrk(password, "abcdefghijklmnopqrstuvwxyz")) score++;
    if (strpbrk(password, "0123456789")) score++;
    if (strpbrk(password, "!@#$%^&*()_+=[]{}|;:,.<>?")) score++;

    if (score >= 5) return "🔥 STRONG";
    else if (score >= 3) return "🟡 MEDIUM";
    else return "🔴 WEAK";
}

// ==================== MAIN PROGRAM ====================
int main() {
    PasswordData data;
    char master_input[50];
    int authenticated = 0;

    printf("🔐 Welcome to Advanced Password Manager v2.0 (C Version) 🔥\n\n");

    load_data(&data);

    // First time setup
    if (strlen(data.master_hash) == 0) {
        printf("=== FIRST TIME SETUP ===\n");
        char master[50], confirm[50];
        while (1) {
            printf("Set a New Master Password: ");
            scanf("%s", master);
            printf("Confirm it: ");
            scanf("%s", confirm);
            if (strcmp(master, confirm) == 0) {
                simple_hash(master, data.master_hash);
                save_data(&data);
                printf("✅ Master Password successfully set!\n\n");
                break;
            } else {
                printf("❌ Passwords do not match. Try again.\n");
            }
        }
    }

    // Authentication
    while (!authenticated) {
        printf("🔑 Enter Master Password: ");
        scanf("%s", master_input);
        char temp_hash[65];
        simple_hash(master_input, temp_hash);
        if (strcmp(temp_hash, data.master_hash) == 0) {
            authenticated = 1;
            printf("✅ Access Granted! Welcome back!\n\n");
        } else {
            printf("❌ Incorrect Master Password! Try again.\n");
        }
    }

    // Main Menu
    while (1) {
        printf("\n==================================================\n");
        printf("                   MAIN MENU\n");
        printf("==================================================\n");
        printf("1. Generate New Password\n");
        printf("2. View All Saved Passwords\n");
        printf("3. Delete a Password\n");
        printf("4. Exit\n");
        printf("==================================================\n");

        int choice;
        printf("\nWhat would you like to do? (1/2/3/4): ");
        scanf("%d", &choice);

        if (choice == 1) {
            char password[PASS_LENGTH + 1];
            int length = 16;
            int use_sym = 1, use_num = 1;

            printf("\n--- New Password Generator ---\n");
            printf("Password length? (Recommended: 16): ");
            scanf("%d", &length);
            if (length < 8) length = 8;

            generate_password(password, length);
            char *strength = check_strength(password);

            printf("\n✅ Generated Password: %s\n", password);
            printf("   Strength: %s\n", strength);

            printf("\nDo you want to save this password? (1=Yes / 0=No): ");
            int save_it;
            scanf("%d", &save_it);
            if (save_it) {
                char account[MAX_LENGTH];
                printf("Account name (Gmail, Instagram etc.): ");
                scanf("%s", account);

                strcpy(data.accounts[data.count].account, account);
                strcpy(data.accounts[data.count].password, password);
                strcpy(data.accounts[data.count].strength, strength);
                time_t now = time(NULL);
                strftime(data.accounts[data.count].created, 20, "%Y-%m-%d %H:%M", localtime(&now));

                data.count++;
                save_data(&data);
                printf("✅ Saved successfully for %s!\n", account);
            }

        } else if (choice == 2) {
            if (data.count == 0) {
                printf("No passwords saved yet.\n");
            } else {
                printf("\n%-20s %-25s %-10s %-15s\n", "Account", "Password", "Strength", "Date");
                printf("-------------------------------------------------------------------\n");
                for (int i = 0; i < data.count; i++) {
                    printf("%-20s %-25s %-10s %-15s\n",
                           data.accounts[i].account,
                           data.accounts[i].password,
                           data.accounts[i].strength,
                           data.accounts[i].created);
                }
            }

        } else if (choice == 3) {
            if (data.count == 0) {
                printf("No passwords to delete.\n");
            } else {
                printf("\nSaved Accounts:\n");
                for (int i = 0; i < data.count; i++) {
                    printf("%d. %s\n", i+1, data.accounts[i].account);
                }
                int idx;
                printf("\nWhich account number to delete?: ");
                scanf("%d", &idx);
                idx--;
                if (idx >= 0 && idx < data.count) {
                    printf("Are you sure you want to delete %s? (1=Yes / 0=No): ", data.accounts[idx].account);
                    int conf;
                    scanf("%d", &conf);
                    if (conf) {
                        for (int i = idx; i < data.count - 1; i++) {
                            data.accounts[i] = data.accounts[i + 1];
                        }
                        data.count--;
                        save_data(&data);
                        printf("🗑️  Deleted successfully!\n");
                    }
                } else {
                    printf("Invalid number!\n");
                }
            }

        } else if (choice == 4) {
            printf("\nThank you for using Advanced Password Manager! 👋\n");
            break;
        } else {
            printf("Invalid choice! Please select 1-4.\n");
        }
    }

    return 0;
}
