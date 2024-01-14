#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 64
#define PORT 4639
#define SA struct sockaddr

// Function designed for chat between client and server.
void handle_connection(unsigned int client_socket) {
	char buff[MAX];
	while (1) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(client_socket, buff, sizeof(buff));
		if (strnlen(buff, MAX) < 1) {
			printf("Client disconnected\n");
			break;
		}

		// print buffer which contains the client contents
		printf("From client: %s\n", buff);
		char *message = "Server: Hello, World!\n";
		write(client_socket, message, sizeof(char) * strlen(message));
	}
}

unsigned int bind_socket() {
	struct sockaddr_in servaddr;
	int server_socket;

	// socket create and verification
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket == -1) {
		fprintf(stderr,"ERROR: Socket creation failed...\n");
		exit(0);
	}

    printf("Socket successfully created...\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(server_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		fprintf(stderr, "ERROR: Socket bind failed...\n");
		exit(0);
	}
	printf("Socket successfully binded...\n");
	return server_socket;
}

int connect_to_client(unsigned int server_socket) {
	unsigned int len, cli, client_socket;

	// Now Server is ready to listen and verification
	if ((listen(server_socket, 5)) != 0) {
		fprintf(stderr, "ERROR: Listen failed...\n");
		exit(0);
		return -1;
	}
	printf("Server listening...\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	client_socket = accept(server_socket, (SA*)&cli, &len);
	if (client_socket < 0) {
		fprintf(stderr, "ERROR: Server accept failed...\n");
		exit(0);
		return -1;
	}
	printf("Server accepted a client...\n");
	return client_socket;
}

int main() {
	unsigned int server_socket = bind_socket();

	while (1) {
		int client_socket = connect_to_client(server_socket);
		if (client_socket == -1) return 0;

		handle_connection(client_socket);
	}

	close(server_socket);
	return 0;
}
