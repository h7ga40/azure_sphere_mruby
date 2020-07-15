#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>

struct passwd {
  char *pw_dir;
};

struct stat {
  mode_t st_mode;
  struct timespec st_mtim;
  off_t st_size;
};
#define st_mtime st_mtim.tv_sec

#define S_IFMT  0170000
#define S_IFDIR 0040000
#define S_IFREG 0100000

#define O_RDONLY  00
#define O_WRONLY  01
#define O_RDWR    02
#define O_CREAT        0100
#define O_TRUNC       01000
#define O_APPEND      02000

#define F_GETFL  3
#define F_SETFL  4

mode_t umask(mode_t mode)
{
	return 0;
}

int chmod(const char *path, mode_t mode)
{
	errno = ENOSYS;
	return -1;
}

char *basename(char *s)
{
	size_t i;
	if (!s || !*s) return ".";
	i = strlen(s)-1;
	for (; i&&s[i]=='/'; i--) s[i] = 0;
	for (; i&&s[i-1]!='/'; i--);
	return s+i;
}

int unlink(const char *path)
{
	errno = ENOSYS;
	return -1;
}

ssize_t readlink(const char *restrict path, char *restrict buf, size_t bufsize)
{
	errno = ENOSYS;
	return -1;
}

int symlink(const char *existing, const char *new)
{
	errno = ENOSYS;
	return -1;
}

int rename(const char *old, const char *new)
{
	errno = ENOSYS;
	return -1;
}

char *realpath(const char *restrict filename, char *restrict resolved)
{
	errno = ENOSYS;
	return NULL;
}

char *getcwd(char *buf, size_t size)
{
	errno = ENOSYS;
	return NULL;
}

struct passwd *getpwnam(const char *name)
{
	errno = ENOSYS;
	return NULL;
}

int flock(int fd, int op)
{
	errno = ENOSYS;
	return -1;
}

char *dirname(char *s)
{
	size_t i;
	if (!s || !*s) return ".";
	i = strlen(s)-1;
	for (; s[i]=='/'; i--) if (!i) return "/";
	for (; s[i]!='/'; i--) if (!i) return ".";
	for (; s[i]=='/'; i--) if (!i) return "/";
	s[i+1] = 0;
	return s;
}

int stat(const char *restrict path, struct stat *restrict buf)
{
	errno = ENOSYS;
	return -1;
}

int fstat(int fd, struct stat *st)
{
	errno = ENOSYS;
	return -1;
}

int lstat(const char *restrict path, struct stat *restrict buf)
{
	errno = ENOSYS;
	return -1;
}

pid_t fork(void)
{
	errno = ENOSYS;
	return -1;
}

int execl(const char *path, const char *argv0, ...)
{
	errno = ENOSYS;
	return -1;
}

_Noreturn void _exit(int status)
{
	errno = ENOSYS;
	while(1);
}

int isatty(int fd)
{
	errno = ENOSYS;
	return -1;
}

int open(const char *filename, int flags, ...)
{
	errno = ENOSYS;
	return -1;
}

pid_t waitpid(pid_t pid, int *status, int options)
{
	errno = ENOSYS;
	return -1;
}

int gethostname(char *name, size_t len)
{
	errno = ENOSYS;
	return -1;
}

int fcntl(int fd, int cmd, ...)
{
	errno = ENOSYS;
	return -1;
}

int ftruncate(int fd, off_t length)
{
	errno = ENOSYS;
	return -1;
}

#include <mruby.h>

extern char _etext, __exidx_start;

mrb_bool mrb_ro_data_p(const char *p)
{
	return (&_etext <= p && p < &__exidx_start);
}
