#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define LOG_PREFIX "log"
#define LOG_EXTENSION ".txt"
#define MAX_FILENAME_LEN 32

FILE *current_log_file = NULL;

// Helper function to check if a string ends with a certain suffix
bool ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return false;
    size_t len_str = strlen(str);
    size_t len_suffix = strlen(suffix);
    if (len_suffix > len_str) return false;
    return strcmp(str + len_str - len_suffix, suffix) == 0;
}

// Find the next log file name
void find_last_log_file(char *new_log_file) {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Failed to open directory");
        exit(1);
    }

    int max_number = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, LOG_PREFIX, strlen(LOG_PREFIX)) == 0 &&
            ends_with(entry->d_name, LOG_EXTENSION)) {

            const char *start = entry->d_name + strlen(LOG_PREFIX);
            char *end;
            int number = strtol(start, &end, 10);

            if (end != start && strcmp(end, LOG_EXTENSION) == 0) {
                if (number > max_number) {
                    max_number = number;
                }
            }
        }
    }
    closedir(dir);

    snprintf(new_log_file, MAX_FILENAME_LEN, "%s%d%s", LOG_PREFIX, max_number + 1, LOG_EXTENSION);
}

// Create a new log file
void create_log_file() {
    char new_log_file[MAX_FILENAME_LEN];
    find_last_log_file(new_log_file);

    current_log_file = fopen(new_log_file, "w");
    if (current_log_file == NULL) {
        perror("Failed to create log file");
        exit(1);
    }
    printf("Log file created: %s\n", new_log_file);
}

// Write a line to the log file
void write_to_log(const char *line) {
    if (current_log_file != NULL) {
        fprintf(current_log_file, "%s\n", line);
        fflush(current_log_file);
    } else {
        fprintf(stderr, "Error: Log file is not open. Call 'create_log_file' first.\n");
        exit(1);
    }
}

// Close the current log file
void close_log_file() {
    if (current_log_file != NULL) {
        fclose(current_log_file);
        current_log_file = NULL;
        printf("Log file closed.\n");
    } else {
        fprintf(stderr, "Error: No log file is open.\n");
        exit(1);
    }
}

// Example usage
int main() {
    create_log_file();
    write_to_log("This is the first log entry.");
    write_to_log("This is the second log entry.");
    close_log_file();

    return 0;
}
