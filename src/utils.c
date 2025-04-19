#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clear_screen() {
    system("clear");
}

void clear_stdin_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int get_string_input(const char *prompt, char *buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        fprintf(stderr, "Error: Invalid buffer provided.\n");
        return 0;
    }

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, buffer_size, stdin) == NULL) {
        buffer[0] = '\0';
        if (feof(stdin)) {
             printf("\nInput stream ended (EOF).\n");
        } else {
             perror("Error reading input");
        }
        clearerr(stdin);
        return 0;
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    size_t len = strlen(buffer);
    if (len == buffer_size - 1 && buffer[len-1] != '\n') {
        clear_stdin_buffer();
    }


    return 1;
}


int get_int_input(const char *prompt, int *value) {
    char buffer[100];
    int success = 0;

    while (!success) {
        if (!get_string_input(prompt, buffer, sizeof(buffer))) {
            return 0;
        }

        if (buffer[0] == '\0') {
            printf("Input cannot be empty. Please enter an integer.\n");
            continue;
        }

        char dummy;
        int result = sscanf(buffer, "%d %c", value, &dummy);

        if (result == 1) {
            success = 1;
        } else if (result == 2) {
             printf("Invalid input. Extra characters after the integer. Please enter only an integer.\n");
        }
         else {
            printf("Invalid input. Please enter a valid integer.\n");
        }
    }

    return 1;
}

void display_header(){
    clear_screen();
    printf("%s    ___  ________________\n   /   |/_  __/ ___/ ___/\n  / /| | / /  \\__ \\\\__ \\ \n / ___ |/ /  ___/ /__/ / \n/_/  |_/_/  /____/____/  \n\x1B[0m",LOGOCOLR);
    printf("\nCopyright (c) 2025  by R Uthaya Murthy, Varghese K James, Tarun S\n");
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
    getchar();
}