#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>



int main(){
    mode_t curr_mask = umask(0000);

    int fd1 = creat("file1", 0777);
    if (fd1 == -1) {
        perror("creat");
        return 1;
    }

    curr_mask = umask(0007);

    int fd2 = creat("file2", 0777);
    if (fd2 == -1) {
        perror("creat");
        return 1;
    }

    return 0;
}