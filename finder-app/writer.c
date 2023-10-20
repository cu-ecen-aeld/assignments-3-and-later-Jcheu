#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	openlog(NULL,0,LOG_USER);
	
	// If is anything other than 2 args, exit
	if(argc != 3)
	{
		syslog(LOG_ERR,"Incorrect # of arguments! Arg#=%d\n", argc);
		syslog(LOG_ERR,"Arguments should be: 1) Filename 2) String to write");
		return 1;
	}
	
	char* filename = argv[1];
	char* stringToWrite = argv[2];
	
	// Assignment 2 debug	
	syslog(LOG_DEBUG, "Writing %s to %s", stringToWrite, filename);
	
	int fd;
	fd = creat(filename, 0644);
	
	//If there is an error creating the filename
	if(fd == -1)
	{
		syslog(LOG_ERR, "Failed to make file!"); 
		return 1;
	}
	
	int nr;
	nr = write(fd, stringToWrite, strlen(stringToWrite));
	
	//If there is an error creating the filename
	if(nr == -1)
	{
		syslog(LOG_ERR, "Failed to write to file!"); 
		return 1;
	}
	
	closelog();
	return 0;
}
