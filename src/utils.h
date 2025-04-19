#ifndef UTILS_H
#define UTILS_H
#define LOGOCOLR "\x1B[36m"

#include <stddef.h>

void clear_screen();
int get_string_input(const char *prompt, char *buffer, size_t buffer_size);
int get_int_input(const char *prompt, int *value);
void clear_stdin_buffer();
void display_header();
void pauseScreen();

#endif