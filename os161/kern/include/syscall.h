#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * 
 * Global locks for system calls
 *
 */

/*
 * global array lock
 */

struct semaphore *arr_lock; 

/*
 * lock for execv
 */

struct lock *exec_lock;        



/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */
//reboot does not return on success
int sys_reboot(int code);

int sys_write(int fd, char *buf, size_t nbytes, int *retval);
int sys_read(int fd, char *buf, size_t buflen, int *retval);

int sys_fork(struct trapframe *tf, int *retval);
int md_forkentry(struct trapframe *tf, unsigned long vmspace);
int sys_getpid(int *retval);
int sys_waitpid(pid_t pid, int *status, int options, int *retval);
int sys__exit(int exit_code);

int sys_execv(const char *program, char **args);

#endif /* _SYSCALL_H_ */
