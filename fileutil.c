#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <libgen.h>
#include <errno.h>

// So here I have defined the structure for nftw
typedef struct {
    int base; // This is the base offset for the path
    int level; // And this is the depth level
} FileTreeWalker;

// Here I have declared some global variables
const char *targetFile; // Target filename
const char *storageDir; // Storage directory
const char *searchExt; // Extension to search
int fileFound = 0; // Flag indicating that the file has been found
const char *commandOption; // Command-line option (-mv or -cp)

// Here I have declared all the functions
void createEmptyTar(const char *dir); // This function is to create an empty tar archive
int searchFileCallback(const char *path, const struct stat *statbuf, int typeflag, FileTreeWalker *ftw); // This is Callback function for searching the file
int searchExtCallback(const char *path, const struct stat *statbuf, int typeflag, FileTreeWalker *ftw); // This is Callback function for searching files with extension

// This is the function to create an empty tar archive
void createEmptyTar(const char *dir) {
    char command[PATH_MAX];
    // Below command creates an empty tar archive
    snprintf(command, sizeof(command), "tar -cf %s/a1.tar --files-from /dev/null", dir);
 
    if (system(command) != 0) {
        perror("system");
        exit(EXIT_FAILURE);
    }
}

// This is the Callback function for searching the file
int searchFileCallback(const char *path, const struct stat *statbuf, int typeflag, FileTreeWalker *ftw) {
    // Checking if it's a regular file and filename matches the target
    if (typeflag == FTW_F && strcmp(path + ftw->base, targetFile) == 0) {
        printf("File located at: %s\n", path);
        fileFound = 1; // Set the file found flag
        // Checking if storage directory is provided
        if (storageDir != NULL) {
            char destPath[PATH_MAX];
            // Generate destination path for copying or moving
            snprintf(destPath, sizeof(destPath), "%s/%s", storageDir, targetFile);
            int srcFd = open(path, O_RDONLY);
            if (srcFd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            int destFd;
            if (strcmp(commandOption, "-mv") == 0) {
                // Move the file
                if (rename(path, destPath) != 0) {
                    perror("rename");
                    close(srcFd);
                    exit(EXIT_FAILURE);
                }
                printf("File moved to: %s\n", destPath);
            } else if (strcmp(commandOption, "-cp") == 0) {
                // Copy the file
                destFd = open(destPath, O_WRONLY | O_CREAT | O_EXCL, statbuf->st_mode);
                if (destFd == -1) {
                    perror("open");
                    close(srcFd);
                    exit(EXIT_FAILURE);
                }
                char buffer[4096];
                ssize_t bytesRead, bytesWritten;
                while ((bytesRead = read(srcFd, buffer, sizeof(buffer))) > 0) {
                    bytesWritten = write(destFd, buffer, bytesRead);
                    if (bytesWritten != bytesRead) {
                        perror("write");
                        close(srcFd);
                        close(destFd);
                        exit(EXIT_FAILURE);
                    }
                }
                close(destFd);
                printf("File copied to: %s\n", destPath);
            } else {
                fprintf(stderr, "Invalid option provided.\n");
                close(srcFd);
                exit(EXIT_FAILURE);
            }
            close(srcFd);
            if (strcmp(commandOption, "-mv") != 0) {
                // Only close destination file descriptor when copying
                close(destFd);
            }
        }
    }
    return 0;
}

// This is the Callback function for searching files with extension
int searchExtCallback(const char *path, const struct stat *statbuf, int typeflag, FileTreeWalker *ftw) {
    // Checking if it's a regular file and contains the specified extension
    if (typeflag == FTW_F && strstr(path, searchExt) != NULL) {
        printf("Matching file found at: %s\n", path);
        // Checking if storage directory is provided
        if (storageDir != NULL) {
            char command[PATH_MAX * 2];
            // Generating the tar command to append file to the archive
            snprintf(command, sizeof(command), "tar -rf %s/a1.tar %s 2>/dev/null", storageDir, path);
            if (system(command) != 0) {
                perror("system");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}

// Main function
int main(int argc, char *argv[]) {
    // Checking if the number of arguments is valid
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage: %s <root_dir> [storage_dir] [options] <filename>\n", argv[0]);
        fprintf(stderr, "       %s [root_dir] [storage_dir] [extension_to_search]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Checking if processing extension search
    if (argc == 4) {
        // Creating an empty tar archive
        createEmptyTar(argv[2]);
        // Setting variables for extension search
        targetFile = NULL;
        storageDir = argv[2];
        searchExt = argv[3];
        commandOption = NULL; // No command option needed for extension search
        // Here is the extension search
        if (nftw(argv[1], searchExtCallback, 20, 0) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        printf("Search for extension completed. Archive created at: %s/a1.tar\n", argv[2]);
    } else {
        targetFile = argv[argc - 1];
        // Checking if storage directory is provided
        if (argc == 5) {
            storageDir = argv[2];
            commandOption = argv[3]; // Command-line option (-mv or -cp)
        } else {
            storageDir = NULL;
            commandOption = NULL;
        }
        // Performing the file search
        if (nftw(argv[1], searchFileCallback, 20, 0) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (!fileFound) {
            printf("Search Failed: File '%s' not found in directory '%s'\n", targetFile, argv[1]);
        }
    }

    return 0;
}

