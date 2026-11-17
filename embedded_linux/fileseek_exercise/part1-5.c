#include <stdio.h>


int main() {
    FILE *fd = fopen("testfile", "w");

    fseek(fd, 1023, SEEK_SET);

    fputc('X',fd);
    
    fclose(fd);

}