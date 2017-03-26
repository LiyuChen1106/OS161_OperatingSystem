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
#include <queue.h>
#include <synch.h>

#define PATH_MAX   1024

/*
 * fork
 * Kernel level code: copy the address space and the trap frame 
 *
 */
int sys_fork(struct trapframe *tf, int *retval) {

    struct thread* child;
    struct addrspace* child_as;

    // create the new address space and frame for the child process
    int return_value = as_copy(curthread -> t_vmspace, &child_as);
    if (return_value) {

        return ENOMEM;
    }

    // copy the trap frame from the parent process to the child process
    struct trapframe *child_frame = kmalloc(sizeof (struct trapframe));
    if (child_frame == NULL) {

        return ENOMEM;
    }

    memcpy(child_frame, tf, sizeof (struct trapframe));
    //*child_frame = *tf; 

    assert(curthread -> t_vmspace != NULL);


    return_value = thread_fork(curthread->t_name, (void*) child_frame,
            (unsigned long) child_as, md_forkentry, &child);

    if (return_value) {
        kfree(child_frame);
        as_destroy(child_as);

        return ENOMEM;
    }

    *retval = child->t_pid;
    return 0;
}

/*
 * User level code for fork
 */
int md_forkentry(struct trapframe *tf, unsigned long vmspace) {

    // make a local trapframe
    struct trapframe child_frame;
    // use the same address space as parent
    struct addrspace *child_addrspace = (struct addrspace*) vmspace;


    memcpy(&child_frame, tf, sizeof (struct trapframe));

    assert(curthread->t_vmspace == NULL);
    curthread->t_vmspace = child_addrspace;
    as_activate(curthread->t_vmspace);

    // change the register values
    child_frame.tf_v0 = 0;
    child_frame.tf_a3 = 0;
    child_frame.tf_epc += 4;


    mips_usermode(&child_frame);
    return 0;

}

/*
 * Getpid returns the process id of the current process.
 */
int sys_getpid(int *retval) {

    *retval = curthread->t_pid;
    return 0;

}

/*
 * This function implement the following user level function:
 * pid_t waitpid(pid_t pid, int *status, int options);
 * It makes the current thread wait for the thread, specified by the 
 * pid being passed in, returning its exit code in the integer pointed to 
 * by status. 
 */
int sys_waitpid(pid_t pid, int *status, int options, int *retval) {

    /* Check for valid parameters */
    if(pid == -8)
	return EINVAL;
    if(pid < 0 || pid > MAX_PID) 
        return EINVAL;
    if(pid == curthread->t_pid)
	return EINVAL;
    if(status == NULL) 
        return EFAULT;
    if(status == MIPS_KSEG0)
	return EFAULT;
    if(status == 0x40000000)
	return EFAULT;
    if((int32_t)status % 4 != 0)
	return EFAULT;
    if(options != 0) 
        return EINVAL;

    

    // Variable for looping through the running threads
    struct thread * check_thread;

    // find the thread that the current thread is waiting for
    P(arr_lock);
    check_thread = active_threads[pid];

    if(check_thread == NULL){

	V(arr_lock);
	return EINVAL;
    }
    V(arr_lock);

    // if the parent id is not matched-> what should be the return value????
    if(check_thread->t_ppid != curthread->t_pid){
	return EINVAL;
    }

    // the thread has not exited
    if (!check_thread-> t_exit) {
	    
        P(check_thread->t_waitsem);
    } 

    // report the exitcode to the calling thread
    P(arr_lock);
    *status = exitcode_table[check_thread->t_pid];

    // child thread has been destroyed
    active_threads[pid] = NULL;
    V(arr_lock);

    // return the exited pid in case of success
    *retval = pid;
    return 0;

}

/*
 * This function implement the following user level function:
 * void _exit(int exitcode);
 * It causes the current process to exit.
 */
int sys__exit(int exit_code) {

	// set up exit code and exit mode for the thread properly
        curthread->t_exitcode = exit_code;
	assert(curthread->t_exit == 0);
	curthread->t_exit = 1;

	// indicate the thread has exited
	P(arr_lock);
	exitcode_table[curthread->t_pid] = exit_code;
	V(arr_lock);

	// release the lock
	V(curthread->t_waitsem);
	
	while(curthread->t_waitsem->count == 1){
		thread_yield();
	}
	
        thread_exit();

     	 // should never reach here
    	 return 0;


}

/*
 * This function replaces the currently executing program with a newly 
 * loaded program image:
 * int execv(const char *program, char **args);
 * 
 */
int sys_execv(const char *program, char **args) {

    int nargs;
    int result;
    int i = 0;

    // for copy program name
    char *name;
    size_t size;
    // for opening file
    struct vnode *v;
    // for define user stack
    vaddr_t entrypoint, stackptr;


    /* Check for valid parameters */
    if(program == NULL)
	    return EFAULT;
    if(program == MIPS_KSEG0)
	    return EFAULT;
    if(program == 0x40000000)
	    return EFAULT;
    if(*program == NULL)
	    return EINVAL;
    if(args == NULL)
	    return EFAULT;
    if(args == MIPS_KSEG0)
	    return EFAULT;
    if(args == 0x40000000)
	    return EFAULT;

    /* Count the number of arguments and copy the arguments */
    while (args[i] != NULL){
	if(args[i] == MIPS_KSEG0)
	    return EFAULT;
	else if(args[i] == 0x40000000)
	    return EFAULT;

        i++;
    }
    nargs = i; 
    //create a local copy for args[]
    char **local_args = kmalloc((nargs+1) * sizeof(char*));

    for (i = 0; i < nargs; i++) {

        int len = strlen(args[i]);
        len++;

        local_args[i] = (char*) kmalloc(len * sizeof (char));
        if (local_args[i] == NULL)
            return ENOMEM;

        result = copyinstr((const_userptr_t) args[i], local_args[i], PATH_MAX, NULL);
        if (result) {
            return EFAULT;
        }

    	local_args[i][len - 1] = '\0';
    }

    local_args[nargs] = NULL; // the last string in the local_args is null??

    
    /* Create a local copy for the program name */
    name = (char *) kmalloc(sizeof (char) * PATH_MAX);
    if (name == NULL)
        return ENOMEM;
    result = copyinstr((const_userptr_t) program, name, PATH_MAX, &size);
    
    
    /* Open the file. */
    result = vfs_open(name, O_RDONLY, &v);
    if (result) {
        return result;
    }
    // destroy it
    if (curthread->t_vmspace != NULL) {
        as_destroy(curthread->t_vmspace);//////////////////////
        curthread->t_vmspace = NULL;
    
    }
    
    /* We should be a new thread. */
    assert(curthread->t_vmspace == NULL);

    /* Create a new address space. */
    curthread->t_vmspace = as_create();
    if (curthread->t_vmspace == NULL) {
        vfs_close(v);
        return ENOMEM;
    }

    /* Activate it. */
    as_activate(curthread->t_vmspace);

    /* Load the executable. */
    result = load_elf(v, &entrypoint);
    if (result) {
        /* thread_exit destroys curthread->t_vmspace */
        vfs_close(v);
        return result;
    }

    /* Done with the file now. */
    vfs_close(v);


    /* Define the user stack in the address space */
    result = as_define_stack(curthread->t_vmspace, &stackptr);
    if (result) {
        /* thread_exit destroys curthread->t_vmspace */
        return result;
    }
    // copy the arguments into the user space
    userptr_t ptr[nargs];
    
    for (i = nargs - 1; i >= 0; i--) {
        int length = strlen(local_args[i]) + 1;
	local_args[i][length-1] = '\0';
        if (length % 4 != 0) {
            length = (length / 4 + 1) * 4;
        }
        stackptr -= length;
        ptr[i] = stackptr;
	
        result = copyoutstr(local_args[i], (userptr_t)stackptr, length, NULL);
        if (result)
            return result;
    }
    
    // make it null terminated
    stackptr -= 4;
    char nullpointer = '\0';
    result = copyout(&nullpointer, (userptr_t)stackptr, 4);
    if(result){
        return result;
    }
    for (i = nargs - 1; i >= 0; i--) {
        stackptr = stackptr - 4;
        copyout(ptr + i, (userptr_t)stackptr, 4);
    }

    /* Warp to user mode. */
    md_usermode(nargs, stackptr, stackptr, entrypoint);

    /* md_usermode does not return */
    panic("md_usermode returned\n");
    return EINVAL;
}
