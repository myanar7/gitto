#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define COMMAND_MAX_LEN 100

int directory_has_git(const char *directory_name) {
    WIN32_FIND_DATA findData;
    char searchPath[MAX_PATH];
    sprintf(searchPath, "%s\\.git", directory_name);
    HANDLE hFind = FindFirstFile(searchPath, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return 1;
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
    SetCurrentDirectory(repository_name);

    // Run git command
    execute_git_command(git_command);

    // Move back to the original directory
    SetCurrentDirectory("..");

    // Add a newline after each repository
    printf("\n");
}

void traverse_directories(const char *git_command) {
    WIN32_FIND_DATA findData;
    char searchPath[MAX_PATH];
    sprintf(searchPath, "%s\\*", ".");
    HANDLE hFind = FindFirstFile(searchPath, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                    handle_repository(findData.cFileName, git_command);
                }
            }
        } while (FindNextFile(hFind, &findData) != 0);
        FindClose(hFind);
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
