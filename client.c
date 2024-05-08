#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>

void calculateFactorialClient(int network_socket);
void getFirstTriangleSide(int network_socket);

void error(const char *msg){
  perror(msg);
  exit(1);
}

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
    error("Error connecting to the server\n");
  }

  char server_response[100];
  
  if (recv(network_socket, server_response, sizeof(server_response), 0) < 0){
    error("There was a problem recieving the response of the server");
  } 
  printf("Server: %s\n", server_response);

  char testing [100] = "Hello Server!!";

  if (send(network_socket, testing, strlen(testing), 0) < 0){
    error("There was an error sending to the server");
  } 

  if (recv( network_socket, server, sizeof(server), 0) < 0) {
    error("There was an error sending to the server");
  }

  printf("Server: %s", server);

  char response;
  int factorial;

  while (1){
    
    memset(server, 0, sizeof(server));
    scanf(" %c", &response);

    send (network_socket, &response, sizeof(response), 0); 
    sleep(2); 
  
    if ( recv(network_socket, server, sizeof(server), 0) < 0){
      error("There was an error sending to the server");
    }

    printf("Server: %s\n", server);
  
    if (strstr(server, "factorial") != NULL) {

     calculateFactorialClient(network_socket); 

     break; 
    }
    else if (strstr (server, "triangle") != NULL) {
       for(int i = 0; i < 3; ++i){
          getFirstTriangleSide(network_socket);
        }

      if ( recv(network_socket, server, sizeof(server), 0) < 0){
        error("There was an error sending to the server");
      }

      printf("\nServer: %s\n", server);
      break;
    }
    else if (strstr (server, "something") != NULL){
     break;
    }
    else{
      printf("Server: please input your choice: ");
      continue;
    }
  }
  
  close(network_socket);

  return 0;
}

void calculateFactorialClient(int network_socket) {
  while (1) {
    // Receive the question from the server
    char question[100];
    char server_response[100];
    recv(network_socket, question, sizeof(question), 0);
    printf("Server: %s", question);

    int number;
    scanf("%d", &number);

    char number_str[100];
    sprintf(number_str, "%d", number);

    send(network_socket, number_str, sizeof(number_str), 0);

    recv(network_socket, server_response, sizeof(server_response), 0);
        
    while (strstr(server_response, "Error") != NULL) {
      getchar(); //clean the input buffer. For some reason fflush(stdin) doesn't work?
      printf("Server: %s\n", server_response); 
      printf("Please enter a valid number: ");

      scanf("%d", &number); 
      sprintf(number_str, "%d", number); 

      send(network_socket, number_str, sizeof(number_str), 0); 
      recv(network_socket, server_response, sizeof(server_response), 0);
    }

    unsigned int result = atoi(server_response);

    printf("Server: Factorial of %d is %d\n", number, result);
    break; // Exit the loop
  }
}

void getFirstTriangleSide(int network_socket){ 
  char question[100];
  char server_response[100];

  // Receive the prompt from the server
  recv(network_socket, question, sizeof(question), 0);
  printf("Server: %s", question);

  // Read the side length from the user
  int number;
  scanf("%d", &number);

  char number_str[100];
  sprintf(number_str, "%d", number);

  send(network_socket, number_str, sizeof(number_str), 0);
}
