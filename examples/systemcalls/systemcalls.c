#include "systemcalls.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    openlog(NULL,0,LOG_USER);
    int rt = system(cmd);
    if(rt == -1)
    {
    	syslog(LOG_ERR, "child process could not be created, or its status could not be retrieved");
	return false;
    }

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    va_end(args);
    openlog(NULL,0,LOG_USER);
    pid_t pid = fork();
    
    //If error on fork
    if(pid == -1)
    {
    	syslog(LOG_ERR, "fork failed");
    	return false;
    }
    else if(pid ==0)
    {
    	//Child action
    	execv(command[0], command);
    	syslog(LOG_ERR, "execv failed");
    	
    	//If it reached here, the execv failed for the child
    	exit(-1);
    }
    
    int status;
    
    if(waitpid(pid, &status, 0) == -1)
    	return false;
    
    //if forked process exited normally
    if (WIFEXITED(status))
    {
	//exit(-1) 8 lowest bits is 255
    	if(WEXITSTATUS(status) == 0xFF)
    	    return false;
    }
    
    //if not successful test command
    if(status != 0)
    	return false;
    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    va_end(args);
    openlog(NULL,0,LOG_USER);
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    
    //If there is an error creating the filename
    if(fd == -1)
    {
	syslog(LOG_ERR, "Failed to make file!"); 
	return false;
    }
    
    pid_t pid = fork();
    
    //If error on fork
    if(pid == -1)
    {
    	syslog(LOG_ERR, "fork failed");
    	return false;
    }
    else if(pid ==0)
    {
    	//Redirect stdout(fd=1) to the file
    	if(dup2(fd, 1) == -1)
    	{
	    syslog(LOG_ERR, "Failed to redirect stdout!"); 
	    return false;
    	}

    	//Child action
    	execv(command[0], command);
    	syslog(LOG_ERR, "execv failed");
    	
    	//If it reached here, the execv failed for the child
    	exit(-1);
    }
    
    int status;
    
    if(waitpid(pid, &status, 0) == -1)
    	return false;
    
    //if forked process exited normally
    if (WIFEXITED(status))
    {
	//exit(-1) 8 lowest bits is 255
    	if(WEXITSTATUS(status) == 0xFF)
    	    return false;
    }
    
    return true;
}
