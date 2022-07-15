#include <stdio.h>
#include <string.h>  
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>


#define SERV_PORT 9999

void sys_err(const char *string)
{

	perror(string);
	exit(1);
}

void catch_child(int num)
{
	while (waitpid(0, NULL, WNOHANG) > 0) // collect child, prevent zombie
	return;
}


int main()
{
	int lfd;
	lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1)
		sys_err("socket error");

	struct sockaddr_in server_address; // IN parameter

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERV_PORT); // little endian to big endian
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY = any avaliable IP in binary format already, then use htonl to big endian

	bind(lfd, (struct sockaddr *)&server_address, sizeof(server_address));

	listen(lfd, 128);

	int cfd;
	struct sockaddr_in client_address; // OUT paramter
	socklen_t client_address_length;   // IN & OUT paramter, so define here instead of passing int accept()
	client_address_length = sizeof(client_address);
	pid_t pid;
	int result;
	char buf[BUFSIZ];

	while(1)
	{
		cfd = accept(lfd, (struct sockaddr *)&client_address, &client_address_length);
		pid = fork();
		if (pid < 0)
			sys_err("fork error");


		else if (pid == 0) // create child process
		{
			close (lfd);

			break;  // wrote it on the bottom to make it easier to read
		}
		else
		{
			struct sigaction action;
			action.sa_handler = catch_child;
			sigemptyset(&action.sa_mask);
			action.sa_flags = 0;

			result = sigaction(SIGCHLD, &action, NULL);
			if (result != 0)
				sys_err("sigaction error");
			close(cfd);
			continue;
		}
	}

	if (pid == 0)
	{
		
		char client_IP[1024]; // if it fine to put it here, every child gets its own version

		printf("client IP: %s PORT: %d\n", 
					inet_ntop(AF_INET, &client_address.sin_addr.s_addr, client_IP, sizeof(client_IP)), // convert IP back from binary to decimal and to little endian
						ntohs(client_address.sin_port)
					  ); 

		while(1)
		{
			// read data from client side, then convert to capital letters, and write back to client 

			char buffer[64] = { 0 };
			int question = read(cfd, buffer, sizeof(buffer)); // read from client
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


			write(cfd, result_buffer, sizeof(result_buffer)); // write to client
		}
	}





	return 0;
}
