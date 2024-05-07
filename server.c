#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

void error (const char *msg){
  perror(msg);
  exit(1);
}

int getValidChoice(int client_socket);
void choiceMenu (int answer, int client_socket);
void calculateFactorial(int client_socket);
bool containsOnlyNumbers(const char *str);
void findTriangleArea(int client_socket);

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
  
  if ( answer == 1){
    calculateFactorial(client_socket);
  }else if ( answer == 2) {
    findTriangleArea(client_socket);
  }
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
     exit(1);
     break;
  }
}

void calculateFactorial(int client_socket){
  unsigned int answer = 1;
  char question[100] = "Input the number of the factorial you want: ";
  char client[100];
  char error[100] = "Error input the number 1 or above\n";
  int number;

  sleep(3);
  send(client_socket, question, sizeof(question), 0);

  while (1) {
    read(client_socket, client, sizeof(client));
    printf("Client: wants the factorial of %s\n", client);

    if (!containsOnlyNumbers(client)) {
      sleep(3);
      send(client_socket, error, sizeof(error), 0);
      continue; 
    }

    number = atoi(client);
    if (number <= 0 || number > 30) {
        sleep(3);
        send(client_socket, error, sizeof(error), 0);
        continue; 
    }

     break; // Exit the loop if valid input received
  }

  for (int i = 1; i <= number; ++i){
    answer *= i;
  }

  char result_str[100];
  sprintf(result_str, "%u", answer);
  send(client_socket, result_str, sizeof(result_str), 0);
}

bool containsOnlyNumbers(const char *str) {

  int lenght = strlen(str);

  for (int i = 0; i < lenght; ++i) {
    if (!isdigit(str[i])) {
      return false;
    }
  }

   return true;
}

void findTriangleArea(int client_socket) {
  char choose1[] = "Please input the first side length: ";
  char choose2[] = "Please input the second side length: ";
  char choose3[] = "Please input the third side length: ";
  char client[100];
  char error[] = "Error: input a number 1 or above\n";
  int sides[3];
  int number;
  int i = 0;
  sleep(3);
  send(client_socket, choose1, sizeof(choose1), 0);
 
  while (1) {
    // Receive the request for the next side from the client
    read(client_socket, client, sizeof(client));
    printf("Client: Side %d: %s\n", i + 1, client);

    if (!containsOnlyNumbers(client)) {
      sleep(3);
      send(client_socket, error, sizeof(error), 0);
      continue;
    }

    number = atoi(client);
    if (number <= 0 || number > 30) {
        sleep(3);
        send(client_socket, error, sizeof(error), 0);
        continue;
    }

    sides[i] = number;

    i++;

    if (i == 1) {
        send(client_socket, choose2, sizeof(choose2), 0);
        continue;
    } else if (i == 2) {
        send(client_socket, choose3, sizeof(choose3), 0);
        continue;
    } else {
       break;
      }
  }

  int a = sides[0], b = sides[1], c = sides[2];
  double s = (a + b + c) / 2.0;
  double area = sqrt(s * (s - a) * (s - b) * (s - c));

  char area_str[100];
  sprintf(area_str, "Area of the triangle: %.2f\n", area);
  send(client_socket, area_str, sizeof(area_str), 0);
}
