#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>


//Open stream socket bound to port 9000 /
//returns -1 for any socket connection failure /
//Listens for and accepts a connections/
//Logs message to syslog "Accepted connection from X IP address of connected cliet/
//Rx data over connection and appends to /var/tmp/aesdsocketdata (create file if doesn't exist)
	//Uses \n to separate data packets
	//Assume no null characters (can use string functions)
	//Data length assumed to be < heap size (malloc size)
//Tx /var/tmp/aesdsocketdata back to client after each packet finish
//Logs message to syslog "Closed connection from X IP address"
//Restarts accepting connections until SIGINT/SIGTERM is received
	//Exits gracefully, completes any open connection operations/closes sockets and deletes /var/tmp/aesdsocketdata
//Logs "Caught signal, exiting" when signals seen


#define OUT_FILE "/var/tmp/aesdsocketdata"

static int serverListening;

void signalHandler(int sigNum)
{
	syslog(LOG_INFO, "Caught signal, exiting");
	serverListening = 0;
}

int main(int argc, char *argv[])
{
	openlog("aesdsocket", LOG_PID,LOG_USER);
	
	int daemonFlag = 0;
	char opt = getopt(argc, argv, "d");
	if(opt == 'd')
	{
		printf("Daemon flag true\n");
		daemonFlag = 1;
	}

	struct sigaction signalAction = {0};
	signalAction.sa_handler = &signalHandler;
	
	//register to the signal handler, if error exit
	if(sigaction(SIGINT, &signalAction, NULL) == -1)
	{
		printf("Error %d, %s registering SIGINT", errno, strerror(errno));
		return -1;
	}
	
	//register to the signal handler, if error exit
	if(sigaction(SIGTERM, &signalAction, NULL) == -1)
	{
		printf("Error %d, %s registering SIGTERM", errno, strerror(errno));
		return -1;
	}
	
	//clear the aesd data file
	FILE * clearFile = fopen(OUT_FILE, "w");
	if(clearFile == NULL)
	{
		printf("Failed to clear out file %d, %s", errno, strerror(errno));
		return -2;
	}
	fclose(clearFile);
	
	//Create Socket
	int serverFD;
	serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if(serverFD == -1)
	{
		printf("Failed to open socket %d, %s", errno, strerror(errno));
		return -1;
	}
	setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	
	//Bind Socket
	struct addrinfo hints = {0};
	struct addrinfo *result;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int addrResult = getaddrinfo(NULL, "9000", &hints, &result);
	if(addrResult !=0)
	{
		printf("Failed getAddrInfo %d\n", addrResult);
		return -1;
	}
	if(bind(serverFD, result->ai_addr, result->ai_addrlen) != 0 )
	{
		printf("Failed to bind socket %d, %s\n", errno, strerror(errno));
		return -1;
	}
	
	freeaddrinfo(result);
	
	//Listen up to 1 connection
	if(listen(serverFD, 1) != 0)
	{
		printf("Failed to listen socket %d, %s\n", errno, strerror(errno));
		return -1;
	}
	
	if(daemonFlag)
	{
		pid_t pid = fork();
    
		//If error on fork
		if(pid == -1)
		{
			syslog(LOG_ERR, "fork failed");
			return -1;
		}
		
		// parent exit
		if (pid > 0) 
		{
			return 0;
		}
		
		//Child
		umask(0);
		setsid();
		chdir("/");
	        close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}
	
	
	serverListening = 1;
	int clientFD;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	while(serverListening != 0)
	{
		syslog(LOG_INFO, "Listening for connections....");
	
		//Accept connection
		clientFD = accept(serverFD, (struct sockaddr*)&clientAddr, &clientAddrLen);
		if(clientFD < 0 )
		{
			//printf("Failed to accept client %d, %s\n", errno, strerror(errno));
			return -1;
		}
		syslog(LOG_INFO, "Accepted connection from %s", inet_ntoa(clientAddr.sin_addr));
		
		//open the aesd data file
		FILE * outFile = fopen(OUT_FILE, "a+");
		if(outFile == NULL)
		{
			//printf("Failed to open out file %d, %s", errno, strerror(errno));
			return -2;
		}
		
		char buffer[1024];
		size_t numBytesRecv;
		int gotNewLine = 0;
		while(gotNewLine == 0)
		{
			numBytesRecv = recv(clientFD, buffer, sizeof(buffer), 0);
			fwrite(buffer, 1, numBytesRecv, outFile);
			//printf("Writing %ld bytes: %s to file", numBytesRecv, buffer);
			
			if(strchr(buffer, '\n') != NULL)
			{
				gotNewLine = 1;
				//printf("Got End of packet\n");
			}
		}
		
		//go back to beginning of file
		rewind(outFile);
		while(fgets(buffer, sizeof(buffer), outFile) != NULL)
		{
			ssize_t numSent = send(clientFD, buffer, strlen(buffer), 0);
			if(numSent < 0)
			{
				printf("send fail");
				//perror("SendSocket");
				return -1;
			}
			//printf("Sending %ld %ld bytes: %s\n", numSent, strlen(buffer), buffer);
		}
		fclose(outFile);
		close(clientFD);
		syslog(LOG_INFO, "Closed connection from %s", inet_ntoa(clientAddr.sin_addr));
	}
	
	//clean up
	close(serverFD);
	closelog();
	return 0;
}
