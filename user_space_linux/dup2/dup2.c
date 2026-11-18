#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
  int fds[2];
  int fds2[2];

  pid_t pid;

  /* Create a pipe.  File descriptors for the two ends of the pipe are
     placed in fds.  */
  pipe(fds);
  pipe(fds2);

  /* Fork a child process.  */
  pid = fork();
  if (pid == (pid_t)0)
  {
    /* This is the child process.  Close our copy of the write end of
       the file descriptor.  */
    close(fds[1]);
    close(fds2[0]);

    /* Connect the read end of the pipe to standard input.  */
    dup2(fds[0], STDIN_FILENO);
    close(fds[0]);

    // connect the write end to the standard output
    dup2(fds2[1], STDOUT_FILENO);
    close(fds2[1]);

    /* Replace the child process with the "sort" program.  */
    execlp("sort", "sort", 0);
  }
  else
  {
    /* This is the parent process.  */
    FILE *stream;
    /* Close our copy of the read end of the file descriptor.  */
    close(fds[0]);
    close(fds2[1]);
    /* Convert the write file descriptor to a FILE object, and write
       to it.  */
    stream = fdopen(fds[1], "w");
    fprintf(stream, "This is a test.\n");
    fprintf(stream, "Hello, world.\n");
    fprintf(stream, "My dog has fleas.\n");
    fprintf(stream, "This program is great.\n");
    fprintf(stream, "One fish, two fish.\n");
    fflush(stream);
    close(fds[1]);

    char buffer[256];
    FILE *read_stream = fdopen(fds2[0], "r");
    if (read_stream != NULL)
    {
      while (fgets(buffer, sizeof(buffer), read_stream) != NULL)
      {
        printf("%s", buffer);
      }
      fclose(read_stream);
    }

    /* Wait for the child process to finish.  */
    waitpid(pid, NULL, 0);
  }

  return 0;
}
