#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>

int main() {

  //create socket
  int network_socket;
  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  //specify address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9002);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  if (connection_status < 0){
    printf( "Error connecting" );
  }

  char server_response[100];
  recv(network_socket, &server_response, sizeof(server_response), 0);

  printf("Server: %s\n", server_response);

  close(network_socket);

  return 0;
}
