#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <kern/unistd.h>
#include <thread.h>
#include <curthread.h>
#include <addrspace.h>
#include <vm.h>
#include <lib.h>
/*
 * This function writes characters to stdout, implementing the following 
 * funtion:
 * int write(int fd, const void *buf, size_t nbytes>);
 * It writes up to buflen bytes to the file specified by fd, at the location 
 * in the current file, taking the data from the space pointed to by buf.
 */

int sys_write(int fd, char* buf, size_t nbytes, int *retval)
{
    
    // check if the buffer is empty
    if(buf == NULL){
	    return EFAULT;
    }

    // check for the file to handle
    if(fd == STDOUT_FILENO || fd == STDERR_FILENO){
        
        // set up a space for write
        char * output = kmalloc(sizeof(char) * (nbytes + 1));
	if(output == NULL){
		return ENOMEM;
	}
        int result = copyin((const_userptr_t)buf, output, nbytes);
        // check if copyin success
        if (result == 0){
            output[nbytes] = '\0';
            int num = kprintf("%s", output);
            *retval = num;
            return result;
        }
        else{ // invalid address
            *retval = 0;
            return result;
        }
    }
    else{ // invalid file
        *retval = 0;
        return EBADF;
    }
}

/*
 * This function reads a character from stdin, implementing the following 
 * funtion:
 * int read(int fd, void *buf, size_t buflen);
 * It reads up to buflen bytes from the file specified by fd, at the location 
 * in the file specified by the current seek position of the file, and 
 * stores them in the space pointed to by buf
 */

int sys_read(int fd, char* buf, size_t buflen, int *retval)
{

    // make sure the arguments are valid
    if(fd != 0 ){
        *retval = -1;
        return EBADF;
    }else if(buf == NULL){
        *retval = -1;
        return EFAULT;
    }else if(buflen <= 0){
        *retval = -1;
        return EIO;
    }else if(buflen>1){
        *retval = -1;
        return ENOSYS;
    }
    
    // read a character from the stdin and write to the pointed file
    char* input = kmalloc(sizeof(char));
    *input = (char)getch();
    
    // copy the char to output file
    int result = copyout(input, (const_userptr_t)buf, buflen);

    
    // set return value
    *retval = sizeof(input);
    return result; 
}
