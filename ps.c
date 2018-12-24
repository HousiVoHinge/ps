#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

void printFileProperties(char *fileName);

void removeAllChars(char *str, char c);

int readFilesFromDir(char* dir);

int main() {
    readFilesFromDir("/proc");
}

void printFileProperties(char *fileName) {
    FILE *filePtr;
    char *linePtr = NULL;
    size_t length = 0;

    char filePath[255];
    strcpy(filePath, "/proc/");
    strcat(filePath, fileName);
    strcat(filePath, "/status");

    filePtr = fopen(filePath, "r");

    // Return when directory has no status file
    if (filePtr == NULL) {
        return;
    }

    char processName[255];
    char processUID[255];
    char processSize[255];
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

    // Convert userID from system to string
    char strUID[12];
    sprintf(strUID, "%d", getuid());

    if (strstr(processUID, strUID) == NULL) {
        return;
    }

    // Remove unnecessary chars for output
    removeAllChars(processName, ' ');
    removeAllChars(processName, '\n');
    removeAllChars(processName, '\t');

    removeAllChars(processSize, ' ');
    removeAllChars(processSize, '\n');
    removeAllChars(processSize, '\t');
    removeAllChars(processSize, 'k');
    removeAllChars(processSize, 'B');


    printf("%s %s %s\n", fileName, processName, processSize);

    if (linePtr) {
        free(linePtr);
    }
}

void removeAllChars(char *str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

int readFilesFromDir(char* dir) {
    DIR *directory;
    struct dirent *subFolder;
    directory = opendir(dir);
    while ((subFolder = readdir(directory)) != NULL) {
        printFileProperties(subFolder->d_name);
    }
    closedir(directory);
}