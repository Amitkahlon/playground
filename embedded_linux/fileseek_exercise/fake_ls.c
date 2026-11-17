#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>




const char PATH[] = "/etc/passwd";

int main() {
    struct stat file_stat;

     if (stat(PATH, &file_stat) == -1) {
        perror("stat failed");
        return 1;
    }

    printf("File size: %lld bytes\n", (long long)file_stat.st_size);
    printf("Permissions (octal): %o\n", file_stat.st_mode & 0777);
    printf("Last modified: %s", ctime(&file_stat.st_mtime));

    
}