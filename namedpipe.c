

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define FIFO_FILE "/tmp/my_fifo"
#define FILE_NAME "hello.txt"

void server();
void client();

int main() {
    pid_t pid;

    // Create FIFO if it doesn't exist
    if (mkfifo(FIFO_FILE, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }

    pid = fork();  // Create child process

    if (pid > 0) {
        // Parent process acts as server
        sleep(1);  // Allow client to start
        server();
        wait(NULL);  // Wait for child
        unlink(FIFO_FILE);  // Remove FIFO
    } 
    else if (pid == 0) {
        // Child process acts as client
        client();
    } 
    else {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

// Server: Reads from hello.txt and writes to FIFO
void server() {
    int fifo_fd, file_fd;
    char buffer[1024];
    ssize_t bytes_read;

    file_fd = open(FILE_NAME, O_RDONLY);
    if (file_fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fifo_fd = open(FIFO_FILE, O_WRONLY);
    if (fifo_fd == -1) {
        perror("Error opening FIFO");
        close(file_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        if (write(fifo_fd, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            break;
        }
    }

    close(file_fd);
    close(fifo_fd);
}

// Client: Reads from FIFO and prints content
void client() {
    int fifo_fd;
    char buffer[1024];
    ssize_t bytes_read;

    fifo_fd = open(FIFO_FILE, O_RDONLY);
    if (fifo_fd == -1) {
        perror("Error opening FIFO");
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
            perror("Write error");
            break;
        }
    }

    close(fifo_fd);
}


