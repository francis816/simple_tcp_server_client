#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h> // for toupper
#include <sys/socket.h>
#include <arpa/inet.h> // for struct sockaddr_in

#define SERVER_PORT 9527

void sys_err(const char* string)
{
	perror(string);
	exit(1);
}

int main()
{
	int listenfd = 0;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) sys_err("socket error");

	struct sockaddr_in server_address; // IN parameter
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT); // little endian to big endian
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY = any avaliable IP in binary format already, then use htonl to big endian
	bind(listenfd, (struct sockaddr *) &server_address, sizeof(server_address));
	
	listen(listenfd, 128);

	int connectfd = 0;
	struct sockaddr_in client_address; // OUT paramter
	socklen_t client_address_length; // IN/OUT paramter so define here instead of passing into accept()
	client_address_length = sizeof(client_address);
	connectfd = accept(listenfd, (struct sockaddr *) &client_address, &client_address_length);
	if (connectfd == -1) sys_err("accept error");
	
	
	char client_IP[1024];

	printf("client IP: %s PORT: %d\n", 
		inet_ntop(AF_INET, &client_address.sin_addr.s_addr, client_IP, sizeof(client_IP)), // convert IP back from binary to decimal and to little endian
		ntohs(client_address.sin_port)
	); 

	
	// read data from client side, then convert to capital letters, and write back to client 
	int result;
	char buffer[BUFSIZ]; // system macro defaults to 4096 or 8192
	while(1)
	{

		result = read(connectfd, buffer, sizeof(buffer));
		write(STDOUT_FILENO, buffer, result);

		for (int i = 0; i < result; i++)
		{
			buffer[i] = toupper(buffer[i]);

		}

		write(connectfd, buffer, result);
	}

	close(connectfd);
	close(listenfd);

	return 0;
}
