#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <windows.h>

#define MAX_SKIP_DIRS 100
#define MAX_DIR_LENGTH MAX_PATH

int readPrerequiredInfo();
void listFilesRecursively(const char *projectPath, char skipDirs[][MAX_DIR_LENGTH], int skipDirCount);
int obfuscateFile(const char *filePath, const char *jsfile);

int main() {

    readPrerequiredInfo();

    return 0;
}

int readPrerequiredInfo() {
    char projectPath[MAX_PATH];
    char skipDirs[MAX_SKIP_DIRS][MAX_DIR_LENGTH];
    int skipDirCount = 0;

    printf("* * * * * * * * JS Obfuscater Tool * * * * * * * *\n");
    printf("--------------------------------------------------\n");
    printf("Enter path for the project directory: ");
    scanf("%s", projectPath);

    // Get the directories to skip from the user
    printf("Enter the directories to be skipped (comma separated): ");
    char skipDirsInput[MAX_DIR_LENGTH * MAX_SKIP_DIRS];
    scanf(" %[^\n]s", skipDirsInput);

    // Split the input string into separate directory names
    char *token = strtok(skipDirsInput, ",");
    while (token != NULL && skipDirCount < MAX_SKIP_DIRS) {
        strncpy(skipDirs[skipDirCount], token, MAX_DIR_LENGTH);
        // Remove any leading or trailing whitespace
        skipDirs[skipDirCount][strcspn(skipDirs[skipDirCount], "\n")] = 0;
        skipDirs[skipDirCount][strcspn(skipDirs[skipDirCount], " ")] = 0;
        token = strtok(NULL, ",");
        skipDirCount++;
    }

    printf("* - - - -  - - Start obfuscating - - - - - - - - -*\n");
    printf("* - - - - - - - - - - - - - - - - - - - - - - - - *\n");
    listFilesRecursively(projectPath, skipDirs, skipDirCount);

    getchar();
    printf("\n# # # FILES SUCCESSFULLY OBFUSCATED # # #");
    getchar();

    return EXIT_SUCCESS;
}

void listFilesRecursively(const char *projectPath, char skipDirs[][MAX_DIR_LENGTH], int skipDirCount) {
    char path[MAX_PATH];
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    snprintf(path, sizeof(path), "%s\\*", projectPath);
  
    hFind = FindFirstFile(path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("FindFirstFile failed (%d)\n", GetLastError());
        return;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) continue;

        // Construct new path for each directory/file
        snprintf(path, sizeof(path), "%s\\%s", projectPath, findFileData.cFileName);

        // If it is a directory, call the function recursively
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Check if the directory should be skipped
            bool skip = false;
            for (int i = 0; i < skipDirCount; i++) {
                if (strcmp(findFileData.cFileName, skipDirs[i]) == 0) {
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                printf("Directory: %s\n", path);
                listFilesRecursively(path, skipDirs, skipDirCount);
            }
        }
        else {
            // If it is a file, check the extension
            const char *extension = strrchr(findFileData.cFileName, '.');
            if (extension && strcmp(extension, ".js") == 0) {
                printf("File: %s", findFileData.cFileName);
                obfuscateFile(path, findFileData.cFileName);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

int obfuscateFile(const char *filePath, const char *jsfile) {
    char command[1024];
    snprintf(command, sizeof(command), "javascript-obfuscator %s --output %s", filePath, filePath);
    int result = system(command);

    if (result == -1){
        perror("system");
        return EXIT_FAILURE;
    }

    printf("DONE!\n\n");
    return EXIT_SUCCESS;
}