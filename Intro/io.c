// A program that does I/O
// Creates a file (/tmp/file) that contains the string "Hello World"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// 3 calls to OS: open(), write(), close()
int main(int argc, char *argv[]) {
    int fd = open("/tmp/file", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); // opens and creates the file
    assert(fd >= 0);
    char buffer[20];
    sprintf(buffer, "hello world\n"); // prints the string "hello world" into buffer 
    int rc = write(fd, buffer, strlen(buffer)); // write data to file
    assert(rc == (strlen(buffer)));
    fsync(fd);
    close(fd); // close the file
    return 0;
}