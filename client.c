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

void sendData(int network_socket, const char *data) {
  if (send(network_socket, data, strlen(data), 0) < 0) {
    error("Error sending data to the server"); 
  }
}

void readData(int network_socket, char *buffer, int buffer_size) {
  if (recv(network_socket, buffer, buffer_size, 0) < 0) {
    error("Error receiving data from the server");
  }
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

  char server_response[150];

  readData (network_socket, server_response, sizeof(server_response));
  printf("Server: %s", server_response);

  char response;
  int factorial;

  while (1){

    bzero(server, sizeof(server));
    scanf("%c", &response);
    send (network_socket, &response, sizeof(response), 0);

    sleep(2);
    readData (network_socket, server, sizeof(server));
    printf("Server: %s\n", server);

    if (strstr(server, "factorial") != NULL) {

     calculateFactorialClient(network_socket);
     break;
    }
    else if (strstr (server, "triangle") != NULL) {
       for(int i = 0; i < 3; ++i){
          sleep(1);
          getFirstTriangleSide(network_socket);
        }

      sleep(1);
      readData (network_socket, server, sizeof(server));
      printf("\nServer: %s\n", server);
      break;
    }
    else if (strstr (server, "Exit") != NULL){
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
    readData (network_socket, question, sizeof(question));
    printf("Server: %s", question);

    int number;
    scanf("%d", &number);

    char number_str[100];
    sprintf(number_str, "%d", number);

    sendData(network_socket, number_str);
    readData(network_socket, server_response, sizeof(server_response));

    while (strstr(server_response, "Error") != NULL) {
       printf("Server: %s\n", server_response);
       printf("Please enter a valid number: ");

       bzero(number_str, sizeof(number_str));

       // Read input as a string again
       scanf("%s", number_str);

       // Convert string to integer again
       number = atoi(number_str);

       // Send the corrected number to the server
       sendData(network_socket, number_str);
       readData(network_socket, server_response, sizeof(server_response));
    }
    unsigned int result = atoi(server_response);

    printf("Server: Factorial of %d is %d\n", number, result);
    break; // Exit the loop
  }
}

void getFirstTriangleSide(int network_socket){
  char question[100];

  // Receive the prompt from the server
  bzero(question, sizeof(question));
  readData (network_socket, question, sizeof(question));
  printf("Server: %s", question);

  // Read the side length from the user
  int number;
  scanf("%d", &number);

  char numberStr[100];
  sprintf(numberStr, "%d", number);
  sendData(network_socket, numberStr);
}
