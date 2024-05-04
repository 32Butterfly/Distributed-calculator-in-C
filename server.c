#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
  
  char server_message[100] = "You have reached the server";

  char client [100];
  int client_response;
  //create socket
  int server_socket;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  //specify address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9002);
  server_address.sin_addr.s_addr = INADDR_ANY;

  bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

  listen(server_socket, 5);

  int client_socket;
  client_socket = accept(server_socket, NULL, NULL);

  //send the message
  send(client_socket, server_message, sizeof(server_message), 0 );

  client_response = read(client_socket, client, 100);

  printf("Client: %s\n", client);

  char testing [150] = "Please choose what kind of calculation you want to perform\n1)Factorial\n2)Triangle area\n3)something?\nPlease input your choice: ";

  send(client_socket, testing, sizeof(testing), 0);

  //close the socket
  close(server_socket);

  return 0;
}
