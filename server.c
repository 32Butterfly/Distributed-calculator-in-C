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

int getValidChoice(int client_socket);
void choiceMenu (int answer, int client_socket);
void calculateFactorial(int client_socket);
bool containsOnlyNumbers(const char *str);
void findTriangleArea(int client_socket);
void typeOfTriangle(int client_socket, int sides[3]);

void error (const char *msg){
  perror(msg);
  exit(1);
}

void sendData(int network_socket, const char *data) {
  if (send(network_socket, data, strlen(data), 0) < 0) {
    error("Error sending data to the client");
  }
}

void readData(int network_socket, char *buffer, int buffer_size) {
  if (recv(network_socket, buffer, buffer_size, 0) < 0) {
    error("Error receiving data from the client");
  }
}

void handleClient(int client_socket) {
  while(1){

    char testing [150] = "Please choose what kind of calculation you want to perform\n1)Factorial\n2)Triangle area\n3)something?\nPlease input your choice: ";

    sendData(client_socket, testing);
    int answer = getValidChoice(client_socket);
    choiceMenu(answer, client_socket);

    if ( answer == 1){
      calculateFactorial(client_socket);
    }
    else if ( answer == 2) {
      findTriangleArea(client_socket);
    }
    else if ( answer == 3){
      break;
    }
  break;
  }
}


int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_len = sizeof(client_address);

  // Create socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    error("Error opening socket");
  }

  // Specify address
  server_address.sin_family = AF_INET;
	  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(9002);

  // Bind socket to address
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    error("Error binding socket");
  }

  // Listen for connections
  listen(server_socket, 5);
  printf("Server listening on port 9002...\n");

  while (1) {
    // Accept incoming connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket < 0) {
       error("Error accepting connection");
    }

     // Create a child process to handle the client
     pid_t pid = fork();
     if (pid < 0) {
       error("Error forking process");
     } else if (pid == 0) {
       // Child process
       close(server_socket); // Close server socket in child process
       handleClient(client_socket);
       close(client_socket); // Close client socket in child process
       exit(0); // Exit child process
     } 
     else {
       // Parent process
       close(client_socket); // Close client socket in parent process
     }
  }

  // Close server socket
  close(server_socket);

  return 0;
}

int getValidChoice(int client_socket){
  int answer;
  char client[10];

  readData(client_socket, client, sizeof(client));
  answer = atoi(client);

   printf("Client chose option: %d\n", answer);

   while(answer < 1 || answer > 3){
     sendData(client_socket, "Incorrect choice\n");
     sleep(2);
     bzero(client, sizeof(client));
     readData(client_socket, client, sizeof(client));
     answer = atoi(client);
   }

  return answer;
}

void choiceMenu(int answer, int client_socket){
  
 switch(answer){
   case 1:
     sendData(client_socket, "Success you chose factorial\n");
     break;
   case 2:
     sendData(client_socket, "Success you chose triangle area\n");
     break;
   case 3:
     sendData(client_socket, "Success you chose something\n");
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
  sendData(client_socket, question);

  while (1) {
    readData(client_socket, client, sizeof(client));
    printf("Client: wants the factorial of %s\n", client);

    if (!containsOnlyNumbers(client)) {
      sleep(3);
      sendData(client_socket, error);
      bzero(client, sizeof(client));
      continue; 
    }

    number = atoi(client);
    if (number <= 0 || number > 12) {
      sleep(3);
      sendData(client_socket, error);
      bzero(client, sizeof(client));
      continue; 
    }

     break; // Exit the loop if valid input received
  }

  for (int i = 1; i <= number; ++i){
    answer *= i;
  }

  char resultStr[100];
  sprintf(resultStr, "%u", answer);
  sendData(client_socket, resultStr);
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
  char choose1[50] = "Please input the first side length: ";
  char choose2[50] = "Please input the second side length: ";
  char choose3[50] = "Please input the third side length: ";
  char client[100];
  char error[] = "Error input a number 1 or above\n";
  int sides[3];
  int number;
  int i = 0;
  sleep(3);
  sendData(client_socket, choose1); 
 
  while (1) {
    // Receive the request for the next side from the client
    readData(client_socket, client, sizeof(client));
    printf("Client: Side %d: %s\n", i + 1, client);

    if (!containsOnlyNumbers(client)) {
      sleep(3);
      sendData(client_socket, error);
      bzero(client, sizeof(client));
      continue;
    }

    number = atoi(client);
    if (number <= 0 || number > 100) {
      sleep(3);
      sendData(client_socket, error);
      bzero(client, sizeof(client));
      continue;
    }

    sides[i] = number;

    if (i == 0) {
      sendData(client_socket, choose2);
      i++;
      continue;
    } 
    else if (i == 1) {
      sendData(client_socket, choose3);
      i++;
      continue;
    } 
    else {
      break;
    }
  }

  int a = sides[0], b = sides[1], c = sides[2];
  double s = (a + b + c) / 2.0;
  double area = sqrt(s * (s - a) * (s - b) * (s - c));

  char areaStr[100];
  sprintf(areaStr, "Area of the triangle: %.2f\n", area);
  sendData(client_socket, areaStr);

  typeOfTriangle(client_socket, sides); 
}

void typeOfTriangle(int client_socket, int sides[3]){
  char equilateral[50] = "The triangle is an equilateral one\n";
  char isosceles [50] = "The triangle is an isosceles one\n";
  char scalene[50] = "The triangle is a scalene one\n"; 

  if ( sides[0] == sides[1] && sides[1] == sides[2]){
    sendData(client_socket, equilateral);
  }
  else if (sides[0] == sides[1] || sides[0] == sides[2] || sides[1] == sides[2]){
    sendData(client_socket, isosceles);
  }
  else{
    sendData(client_socket, scalene);
  }
}
