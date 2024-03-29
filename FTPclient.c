// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>

#include <FTPclient.h>
#include <helpers.h>

void localCommand(char* input, struct CommandRegex commands){
    char *cwd = (char *) malloc(sizeof(char) * 1024);
    getcwd(cwd, 1024);
    if(checkRegex(commands.LS, input)){
	printf("%s", lsCommand(cwd));
    } else if (checkRegex(commands.PWD, input)) {
	printf("%s", pwdCommand(cwd));
    } else if (checkRegex(commands.CD, input)) {
	char* dir = stripStartingChars(commands.cd_len, input);
	int ret = chdir(dir);
	if(ret == 0){ // If the directory is valid
	    getcwd(cwd, 1024);
	    // Return the pwd command so they know where they changed to
	    printf("%s", pwdCommand(cwd));
	} else {
	    printf("Invalid target directory!\n");
	}
    }
    return;
}

int main(int argc, char const *argv[])
{
    const char *ip_addr = argv[1];
    struct CommandRegex commands = compileAllCommandChecks();
    int PORT = atoi(argv[2]);
    struct sockaddr_in address;
    char buffer[10000] = {0};
    int valread;
    int sock = connectToSocket(ip_addr, PORT);
    int data_port;
    int a_len = sizeof(address);
    getsockname(sock, (struct sockaddr *)&address, (socklen_t*)&a_len);
    char data_ip[1024];
    data_port = ntohs(address.sin_port)+1;
    struct SetupVals setup = setupAndBind(data_port, 1);
    char input[128];
    while(1){
	memset(&buffer[0], 0, sizeof(buffer));
	memset(&input[0], 0, sizeof(input));
	printf("ftp> ");
	fgets(input, 128, stdin);
	if(input[0] == '!'){
	    // Remove the local spec
	    char* local_input = stripStartingChars(1, input);
	    localCommand(local_input, commands);
	}
	else {	    
	    if(checkRegex(commands.GET, input) || checkRegex(commands.PUT, input)){
		fd_set tracker;
		int new_socket;
		FD_SET(setup.server_fd, &tracker);
		send(sock, input, strlen(input), 0);
		select(setup.server_fd+1, &tracker, NULL, NULL, NULL);
		if ((new_socket = accept(setup.server_fd, (struct sockaddr *)&setup.address,(socklen_t*)&setup.addrlen))<0) {
		    perror("accept");
		    exit(EXIT_FAILURE);
		}
		char path[1024];
		char *cwd = (char *) malloc(sizeof(char) * 1024);
		getcwd(cwd, 1024);
		strcpy(path, cwd);
		strcat(path, "/");
		strcat(path, stripStartingChars(commands.get_len, input));
		if(checkRegex(commands.PUT, input)){
		    transferFile(new_socket, path);
		} else {
		    receiveFile(new_socket, path);
		}
	    } else {
		send(sock, input, strlen(input), 0);
	    }
	    valread = read(sock , buffer, 10000);
	    printf("%s", buffer);
	    if(checkRegex(commands.QUIT, input)){
		return 0;
	    }
	}
    }
    return 0;
}
