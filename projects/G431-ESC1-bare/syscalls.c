// syscalls.c — minimal libc syscall stubs for bare-metal newlib-nano

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int _close(int fd) {
    (void)fd;
    errno = EBADF;
    return -1;
}

int _lseek(int fd, int offset, int whence) {
    (void)fd; (void)offset; (void)whence;
    errno = ESPIPE;
    return -1;
}

int _read(int fd, char *ptr, int len) {
    (void)fd; (void)ptr; (void)len;
    errno = EBADF;
    return -1;   // no stdin for now; revisit if you wire up RX
}

int _fstat(int fd, struct stat *st) {
    (void)fd;
    st->st_mode = S_IFCHR;   // pretend everything is a character device
    return 0;
}

int _isatty(int fd) {
    return (fd == STDOUT_FILENO || fd == STDERR_FILENO) ? 1 : 0;
}

int _getpid(void) {
    return 1;
}

int _kill(int pid, int sig) {
    (void)pid; (void)sig;
    errno = EINVAL;
    return -1;
}

void _exit(int status) {
    (void)status;
    while (1) { /* trap — nowhere to return to on bare metal */ }
}