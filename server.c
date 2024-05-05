#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>

void error (const char *msg){
  perror(msg);
  exit(1);
}

int getValidChoice(int client_socket);
void choiceMenu (int answer, int client_socket);
void calculateFactorial(int client_socket);


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
  
  if (client_socket < 0){
    error("The was a problem connecting to the socket\n");
  }

  //send the message
  send(client_socket, server_message, sizeof(server_message), 0 );

  client_response = read(client_socket, client, 100);

  if (client_response < 0){
    error("There was a problem reading data\n");
  }

  printf("Client: %s\n", client);

  while(1){

  char testing [150] = "Please choose what kind of calculation you want to perform\n1)Factorial\n2)Triangle area\n3)something?\nPlease input your choice: ";

  send(client_socket, testing, sizeof(testing), 0);

  int answer = getValidChoice(client_socket);

  choiceMenu(answer, client_socket);
  
  calculateFactorial(client_socket);

  break;
 
 }
  //close the socket
  close(server_socket);

  return 0;
}

int getValidChoice(int client_socket){
  
  int answer = 0;
  char client[10];  

  do {
    
    memset(client, 0, sizeof(client));
    read(client_socket, client, sizeof(client));
    answer = atoi(client);

    printf("Client chose option: %d\n", answer);

    if (answer < 1 || answer > 3) {
      send(client_socket, "Incorrect choice\n", sizeof("Incorrect choice\n"), 0);
      sleep(2);
    }

  }while (answer < 1 || answer > 3);

  return answer;
}

void choiceMenu(int answer, int client_socket){
  
 switch(answer){
   case 1:
      send(client_socket, "Success you chose factorial\n", sizeof("Success you chose factorial\n"), 0); 
      break;
   case 2:
      send(client_socket, "Success you chose triangle area\n", sizeof("Success you chose triangle area\n"), 0);
      break;
   case 3:
      send(client_socket, "Success you chose something\n", sizeof("Success you chose something\n"), 0);
      break;
   default:
     send(client_socket, "Incorrect choice\nPlease input your choice\n", sizeof("Incorrect choice\nPlease input your choice\n"), 0);
     break;
  }
}

void calculateFactorial(int client_socket){
    
  unsigned int answer = 1;
  char question [100] = "Input the number of the factorial you want: ";
  char client [100];
  char error [100] = "Error: input the number 1 or above";
  int number ;

  sleep(3);
  send(client_socket, question, sizeof(question), 0);

  read(client_socket, client, sizeof(client));

  printf ("Client: wants the factorial of %s\n", client);
 
  number = atoi(client);
  
  while (number <= 0 || number > 30){
    sleep(3);
    send(client_socket, error, sizeof(error), 0);

    read(client_socket, client, sizeof(client));

    number = atoi(client);
  }

  for (int i = 1; i <= number; ++i){
    answer *= i;
  }

  send (client_socket, &answer, sizeof(answer), 0);
}
