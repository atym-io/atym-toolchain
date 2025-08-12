#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>

#define CWD "/"
#define FOLDER_PATH CWD "folder"
#define FILE_PATH   CWD "folder/test.txt"
#define BUF_SIZE 512

// Generates a pseudo-random temp filename inside /folder
void generate_temp_filename(char *buffer, size_t size) {
    int suffix = rand() % 10000;
    snprintf(buffer, size, "%s/tempfile-%d.txt", FOLDER_PATH, suffix);
}

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);  // Disable stdout buffering
    srand((unsigned)time(NULL));       // Seed random generator

    int rc;
    struct stat st;

    printf("LibC / POSIX File API Demo\n");

    // Check if directory exists
    rc = stat(FOLDER_PATH, &st);
    if (rc == 0 && S_ISDIR(st.st_mode)) {
        printf("Directory exists: %s\n", FOLDER_PATH);

        // Attempt to delete test file
        rc = unlink(FILE_PATH);
        if (rc != 0 && errno != ENOENT) {
            printf("Failed to remove file \"%s\": %s (%d)\n", FILE_PATH, strerror(errno), errno);
            return -1;
        }

        // Attempt to remove directory
        rc = rmdir(FOLDER_PATH);
        if (rc != 0) {
            printf("Failed to remove directory \"%s\": %s (%d)\n", FOLDER_PATH, strerror(errno), errno);
            return -1;
        }

        printf("Existing directory removed.\n");
    }
    else if (rc != 0 && errno != ENOENT) {
        printf("stat failed for path \"%s\": %s (%d)\n", FOLDER_PATH, strerror(errno), errno);
        return -1;
    }

    // Create the directory
    rc = mkdir(FOLDER_PATH, 0777);
    if (rc != 0) {
        printf("mkdir failed for \"%s\": %s (%d)\n", FOLDER_PATH, strerror(errno), errno);
        return -1;
    }
    printf("mkdir success: %s\n", FOLDER_PATH);

    // Open test file
    FILE *file = fopen(FILE_PATH, "w+");
    if (!file) {
        printf("fopen failed for \"%s\": %s (%d)\n", FILE_PATH, strerror(errno), errno);
        return -1;
    }
    printf("fopen success: %s\n", FILE_PATH);

    // Write to test file
    const char *data = "Hello, World!";
    size_t nitems = fwrite(data, sizeof(char), strlen(data) + 1, file);
    printf("fwrite returned %zu bytes\n", nitems);

    // Read file back
    rc = fseek(file, 0, SEEK_SET);
    printf("fseek returned %d\n", rc);

    char buffer[32];
    nitems = fread(buffer, sizeof(char), sizeof(buffer), file);
    printf("fread: %zu bytes\n", nitems);
    printf("buffer read = %s\n", buffer);

    rc = fclose(file);
    printf("fclose returned %d\n", rc);

    // List contents of the directory
    printf("opendir: %s\n", FOLDER_PATH);
    DIR *dir = opendir(FOLDER_PATH);
    if (!dir) {
        printf("opendir failed for \"%s\": %s (%d)\n", FOLDER_PATH, strerror(errno), errno);
        return -1;
    }

    printf("Directory listing for: %s\n", FOLDER_PATH);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("  %s (type: %d)\n", entry->d_name, entry->d_type);
    }

    rc = closedir(dir);
    if (rc != 0) {
        printf("closedir failed: %s (%d)\n", strerror(errno), errno);
        return -1;
    }

    // Fallback temp file demo
    char tmpname[64];
    generate_temp_filename(tmpname, sizeof(tmpname));

    FILE *tmp = fopen(tmpname, "w+");
    if (!tmp) {
        printf("Fallback temp file failed: %s (%d)\n", strerror(errno), errno);
    } else {
        fputs("Temporary data\n", tmp);
        rewind(tmp);
        fgets(buffer, sizeof(buffer), tmp);
        printf("Fallback temp file read: %s", buffer);
        fclose(tmp);
        unlink(tmpname);  // Clean up
    }

    return 0;
}
