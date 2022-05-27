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
	while(1)
	{

		char buffer[64] = { 0 };
		int question = read(connectfd, buffer, sizeof(buffer)); // read from client
		write(STDOUT_FILENO, buffer, question); // write to screen

		double num1 = 0;
		double num2 = 0;
		char ch = 0;

		sscanf(buffer, "Client: What is %lf %c %lf = ? \n", &num1, &ch, &num2);

		double result = 0;

		switch(ch)
		{
		case '+':
			result = num1 + num2;
			break;
		case '-':
			result = num1 - num2;
			break;
		case '*':
			result = num1 * num2;
			break;
		case '/':
			result = num1 / num2;
			break;
		default:
			break;
		}

		char result_buffer[64] = { 0 };
		sprintf(result_buffer, "----Server replied: %.2lf %c %.2lf = %.2lf\n", num1, ch, num2, result);


		write(connectfd, result_buffer, sizeof(result_buffer)); // write to client
	}

	close(connectfd);
	close(listenfd);

	return 0;
}
