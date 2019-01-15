#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

void printProcessProperties(char *processID);

void removeChars(char *string, char c);

int readFilesFromDir(char *dirPath);

int main() {
    readFilesFromDir("/proc");
}

/**
 * Reads the status file of the given process and prints its ID, name and size.
 * @param processID: Print properties of this process
*/
void printProcessProperties(char *processID) {
    FILE *filePtr;
    char *linePtr = NULL;
    size_t length = 0;

    // Open the status file of the process
    char filePath[255];
    strcpy(filePath, "/proc/");
    strcat(filePath, processID);
    strcat(filePath, "/status");
    filePtr = fopen(filePath, "r");

    // Return when directory has no status file
    if (filePtr == NULL) {
        return;
    }

    char processName[255];
    char processUID[255];
    char processSize[255] = "0";

    // Loop over all lines of the status file and extract wanted data
    while ((getline(&linePtr, &length, filePtr)) != -1) {
        if (strstr(linePtr, "Name") != NULL) {
            strcpy(processName, strstr(linePtr, ":") + 1);
        }
        if (strstr(linePtr, "Uid") != NULL) {
            strcpy(processUID, strstr(linePtr, ":") + 1);
        }
        if (strstr(linePtr, "VmRSS") != NULL) {
            strcpy(processSize, strstr(linePtr, ":") + 1);
        }
    }

    fclose(filePtr);
    free(linePtr);

    // Convert userID from system to string
    char strUID[12];
    sprintf(strUID, "%d", getuid());

    // If the process is not owned by the current user, return.
    if (strstr(processUID, strUID) == NULL) {
        return;
    }

    // Remove unnecessary chars for output
    removeChars(processName, ' ');
    removeChars(processName, '\n');
    removeChars(processName, '\t');

    removeChars(processSize, ' ');
    removeChars(processSize, '\n');
    removeChars(processSize, '\t');
    removeChars(processSize, 'k');
    removeChars(processSize, 'B');

    // Print final output
    printf("%s %s %s\n", processID, processName, processSize);
}

/**
 * Removes all occurrences of the given character from the string.
 * @param string: String which is shortened
 * @param c: Character to remove
*/
void removeChars(char *string, char c) {
    char *pr = string;
    char *pw = string;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

/**
 * Reads all files from a directory.
 * @param dirPath: Read files from the directory at this path
*/
int readFilesFromDir(char *dirPath) {
    DIR *directory;
    struct dirent *subFolder;
    directory = opendir(dirPath);
    while ((subFolder = readdir(directory)) != NULL) {
        printProcessProperties(subFolder->d_name);
    }
    closedir(directory);
}