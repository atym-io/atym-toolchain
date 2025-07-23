#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

// #include <wasi/api.h>

// We are expecting to work at the root dir
#define CWD "/"
#define FOLDER_PATH CWD "folder"
#define FILE_PATH   CWD "folder/test.txt"
#define BUF_SIZE 512

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);             // Logs don't show up reliably so disable stdout buffering

    int rc;
    struct stat st;

    printf("Wasm filesystem demo\n");

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
        // stat() failed for unexpected reason
        printf("stat failed for path \"%s\": %s (%d)\n", FOLDER_PATH, strerror(errno), errno);
        return -1;
    }

    // Now create the directory
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

    // Close the file
    rc = fclose(file);
    printf("fclose returned %d\n", rc);

    // List contents of the directory
    printf("opendir: %s\n", FOLDER_PATH);
    DIR *dir = opendir(FOLDER_PATH);
    if (!dir) {
        printf("opendir failed for \"%s\": %s (%d)\n", FOLDER_PATH, strerror(errno), errno);
        return -1;
    }
    printf("opendir OK! dir = %p\n", dir);

    printf("Directory listing for: %s\n", FOLDER_PATH);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("  %s (type: %d)\n", entry->d_name, entry->d_type);
    }

    printf("closedir\n");
    fflush(stdout);
    int closed = closedir(dir);
    if (closed != 0) {
        printf("closedir failed: %s (%d)\n", strerror(errno), errno);
        return -1;
    }

    return 0;
}

// int main(void) {
//     setvbuf(stdout, NULL, _IONBF, 0);             // Logs don't show up reliably so disable stdout buffering

//     printf("Wasm filesystem demo\n");

//     printf("Opening directory: %s\n", FOLDER_PATH);

//     // Use open() with O_DIRECTORY to get a raw WASI fd
//     int fd = open(FOLDER_PATH, O_RDONLY | O_DIRECTORY);
//     if (fd < 0) {
//         printf("open failed: %s (%d)\n", strerror(errno), errno);
//         return -1;
//     }

//     printf("open OK! fd = %d\n", fd);

//     uint8_t buf[BUF_SIZE];
//     size_t bufused = 0;
//     __wasi_dircookie_t cookie = 0;

//     __wasi_errno_t err = __wasi_fd_readdir(fd, buf, BUF_SIZE, cookie, &bufused);
//     if (err != 0) {
//         printf("fd_readdir failed: %d\n", err);
//         close(fd);
//         return -1;
//     }

//     printf("Directory listing for fd=%d:\n", fd);

//     size_t offset = 0;
//     while (offset + sizeof(__wasi_dirent_t) <= bufused) {
//         __wasi_dirent_t *wdirent = (__wasi_dirent_t *)(buf + offset);
//         const char *name = (const char *)(buf + offset + sizeof(__wasi_dirent_t));

//         printf("  %.*s (type: %d)\n", (int)wdirent->d_namlen, name, wdirent->d_type);

//         offset += sizeof(__wasi_dirent_t) + wdirent->d_namlen;
//         cookie = wdirent->d_next;
//     }

//     close(fd);
//     return 0;
// }

