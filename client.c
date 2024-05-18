#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>

int network_socket;

void calculateFactorialClient(int network_socket);
void getFirstTriangleSide(int network_socket);

void error(const char *msg){
  perror(msg);
  exit(1);
}

void sendData(int network_socket, const char *data) {
  sleep(1);
  if (send(network_socket, data, strlen(data), 0) < 0) {
    error("Error sending data to the server"); 
  }
}

void readData(int network_socket, char *buffer, int buffer_size) {
  sleep(1);
  if (recv(network_socket, buffer, buffer_size, 0) < 0) {
    error("Error receiving data from the server");
  }
}

void handle_sigint(int sig) {
  printf("\nCaught signal. Closing the client and child server\n");
  close(network_socket);
  exit(0);
}

int main(int argc, char *argv[]) {
  int server_success, portnumber;
  signal(SIGINT, handle_sigint);

  if (argc < 3){
    fprintf(stderr, "Usage: %s hostname portnumber\n", argv[0]);
    exit(1);
  }
 
  char server [150];
  //create socket
  network_socket = socket(AF_INET, SOCK_STREAM, 0);

  portnumber = atoi(argv[2]);
  struct hostent *Server = gethostbyname(argv[1]);
    if (Server == NULL) {
        fprintf(stderr, "Error: No such host exists\n");
        exit(1);
    }

  //specify address
  struct sockaddr_in server_address;
  Server = gethostbyname(argv[1]);
  bzero(( char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portnumber);
  bcopy((char *)Server->h_addr, (char *)&server_address.sin_addr.s_addr, Server->h_length);

  int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

  if (connection_status < 0){
    error("Error connecting to the server\n");
  }

  char server_response[200];
  readData (network_socket, server_response, sizeof(server_response));

  char response[5];

  while (1){
    printf("Server: %s", server_response);
    scanf("%s", &response);
    sendData(network_socket, response);

    readData (network_socket, server, sizeof(server));
    printf("Server: %s\n", server);

    if (strstr(server, "factorial") != NULL) {
     calculateFactorialClient(network_socket);
     bzero(server, sizeof(server));
     bzero(response, sizeof(response));
    }
    else if (strstr (server, "triangle") != NULL) { 
      bzero(server, sizeof(server));
      getFirstTriangleSide(network_socket);
      readData (network_socket, server, sizeof(server));
      printf("\nServer: %s", server);
      bzero(server, sizeof(server));
      readData (network_socket, server, sizeof(server));
      printf("Server: %s", server);
      bzero(server, sizeof(server));
    }
    else if (strstr (server, "Exit") != NULL){
     exit(1);
    }
    else{
      printf("Server: please input your choice: ");
      bzero(server, sizeof(server));
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
    bzero(&number, sizeof(number));
    bzero(&result, sizeof(result));
    bzero(question, sizeof(question));
    bzero(server_response, sizeof(server_response));
    bzero(number_str, sizeof(number_str));
    break; // Exit the loop
  }
}

void getFirstTriangleSide(int network_socket){
  char question[100];
  int i = 0;

  // Receive the prompt from the server
  readData (network_socket, question, sizeof(question));
  int number;

  while(i < 3){
  // Read the side length from the user
  printf("Server: %s", question);
  scanf("%d", &number);

  char numberStr[100];
  sprintf(numberStr, "%d", number);
  sendData(network_socket, numberStr);
  bzero(question, sizeof(question));
  if ( i == 2) break;
  readData(network_socket, question, sizeof(question));
  while (strstr (question, "Error") != NULL){
     printf("Server: %s", question);
     scanf("%d", &number);

     sprintf(numberStr, "%d", number);
     sendData(network_socket, numberStr);
     bzero(question, sizeof(question));
     readData(network_socket, question, sizeof(question));
    }
   i++;
  }
  bzero(question, sizeof(question));
}
