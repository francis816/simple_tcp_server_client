#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 9527 
void sys_err(const char *string)
{
	perror(string);
	exit(1);
}

int main()
{
	int connectfd;

	connectfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connectfd == -1) sys_err("socker error");

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);  // match with the server one, little to big endian
	inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr.s_addr); // convert server ip from string to int, also little to big endian


	int result = connect(connectfd, (struct sockaddr *)&server_address, sizeof(server_address));
	if (result != 0) sys_err("connect err"); //sueccess = 0, error = -1
	
	printf("hihi");
	char buffer[BUFSIZ];
	int i = 10;
	while(--i)
	{
		write(connectfd, "hello\n", 6); // write to fd
		result = read(connectfd, buffer, sizeof(buffer)); // read from fd
		write(STDOUT_FILENO, buffer, result); // write to screen
		sleep(1);
	}
	
	close (connectfd);
	
	return 0;
}
