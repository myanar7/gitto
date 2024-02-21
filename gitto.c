#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define COMMAND_MAX_LEN 100

int directory_has_git(const char *directory_name) {
    DIR *directory;
    struct dirent *entry;

    directory = opendir(directory_name);
    if (directory != NULL) {
        while ((entry = readdir(directory)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".git") == 0) {
                closedir(directory);
                return 1;
            }
        }
        closedir(directory);
    }
    return 0;
}

void execute_git_command(const char *command) {
    char buffer[COMMAND_MAX_LEN];
    snprintf(buffer, COMMAND_MAX_LEN, "git %s", command);
    system(buffer);
}

void handle_repository(const char *repository_name, const char *git_command) {
    // Check if the directory has a .git file
    if (!directory_has_git(repository_name)) {
        return;
    }
    
    // Print the repository name
    printf("Repository: %s\n", repository_name);

    // Enter the directory
    chdir(repository_name);

    // Run git command
    execute_git_command(git_command);

    // Move back to the original directory
    chdir("..");

    // Add a newline after each repository
    printf("\n");
}

void traverse_directories(const char *git_command) {
    DIR *directory;
    struct dirent *entry;

    directory = opendir(".");
    if (directory != NULL) {
        while ((entry = readdir(directory)) != NULL) {
            if (entry->d_type == DT_DIR) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    handle_repository(entry->d_name, git_command);
                }
            }
        }
        closedir(directory);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <git_command>\n", argv[0]);
        return 1;
    }

    // Combine all arguments after the first one into a single string
    int total_length = 0;
    for (int i = 1; i < argc; ++i) {
        total_length += strlen(argv[i]) + 1; // +1 for space
    }
    char git_command[total_length];
    git_command[0] = '\0'; // Ensure the string is empty initially
    for (int i = 1; i < argc; ++i) {
        strcat(git_command, argv[i]);
        if (i != argc - 1) {
            strcat(git_command, " "); // Add space between arguments
        }
    }

    printf("Executing 'git %s' for all repositories in the current directory...\n\n", git_command);
    traverse_directories(git_command);

    return 0;
}
