#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 9527 


void sys_err(const char *string)
{
	perror(string);
	exit(1);
}

int main(int argc, char *argv[])
{
	int connectfd;

	connectfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connectfd == -1) sys_err("socker error");

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);  // match with the server one, little to big endian
	inet_pton(AF_INET, argv[1], &server_address.sin_addr.s_addr); // convert server ip from string to int, also little to big endian


	int result = connect(connectfd, (struct sockaddr *)&server_address, sizeof(server_address));
	if (result != 0) sys_err("connect err"); //sueccess = 0, error = -1
	
	srand((size_t)time(NULL));
	int num1 = 0;
	int num2 = 0;
	char ch = 0;
	char operator[] = { '+', '-', '*', '/' };


	for (int i = 0; i < 20; i++)
	{
		num1 = rand() % 100 + 1;
		num2 = rand() % 100 + 1;
		ch = operator[ rand() % 4 ];
		
		char buffer[64] = { 0 } ;
		sprintf(buffer, "Client: What is %d %c %d = ? \n", num1, ch, num2);
		
		write(connectfd, buffer, sizeof(buffer)); // write to server
		result = read(connectfd, buffer, sizeof(buffer)); // read from server
		write(STDOUT_FILENO, buffer, result); // write to screen
		sleep(1);
	}
	
	close (connectfd);
	
	return 0;
}
