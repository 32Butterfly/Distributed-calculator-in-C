#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>


void calculateFactorialClient(int network_socket);

int main() {

  //create socket
  int network_socket;
  int server_success;

  char server [150];
  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  //specify address
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9002);
  server_address.sin_addr.s_addr = INADDR_ANY;

  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  if (connection_status < 0){
    printf( "Error connecting to the server\n" );
    exit(1);
  }

  char server_response[100];
  recv(network_socket, server_response, sizeof(server_response), 0);

  printf("Server: %s\n", server_response);

  char testing [100] = "Hello Server!!";

  send(network_socket, testing, strlen(testing), 0); 

  server_success = recv( network_socket, server, sizeof(server), 0);

  printf("Server: %s", server);

  char response;
  int factorial;

  while (1){
    
    memset(server, 0, sizeof(server));
    scanf(" %c", &response);

    send (network_socket, &response, sizeof(response), 0); 
    sleep(2); 
  
    recv(network_socket, server, sizeof(server), 0);

    printf("Server: %s\n", server);
  
    if (strstr(server, "factorial") != NULL) {

     calculateFactorialClient(network_socket); 

     break; 
    }

    if (strstr(server, "Incorrect choice") != NULL) {
        printf("Please input your choice: ");
    }
  
   break; 
  }
  
  close(network_socket);

  return 0;
}


void calculateFactorialClient(int network_socket) {
  while (1) { 
      // Receive the question from the server
      char question[100];
      recv(network_socket, question, sizeof(question), 0);
      printf("Server: %s", question);

      int number;
      scanf("%d", &number);

      char number_str[100];
      sprintf(number_str, "%d", number);

      send(network_socket, number_str, sizeof(number_str), 0);

      unsigned int result;
      recv(network_socket, &result, sizeof(result), 0);
        
      printf("Server: Factorial of %d is %u\n", number, result);
      break; // Exit the loop
   } 
}
