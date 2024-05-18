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
#include <signal.h>

int getValidChoice(int client_socket, int server[2], int child[2]);
void choiceMenu (int answer, int client_socket);
void calculateFactorial(int client_socket, int server[2], int child[2]);
bool containsOnlyNumbers(const char *str);
void findTriangleArea(int client_socket, int server[2], int child[2]);
void typeOfTriangle(int client_socket, int server[2], int child[2], int sides[3]);

unsigned int factorial(int n) {
  unsigned int answer = 1;
  if (n == 0 || n == 1){
      return 1;
  }
   else {
      for(int i = 1; i <= n; ++i){
        answer *= i;
   }

   return answer;
  }
}

void error (const char *msg){
  perror(msg);
  exit(1);
}

void sendData(int network_socket, const char *data) {
  sleep(1);
  if (send(network_socket, data, strlen(data), 0) < 0) {
    error("Error sending data to the client");
  }
}

void readData(int network_socket, char *buffer, int buffer_size) {
  sleep(1);
  if (recv(network_socket, buffer, buffer_size, 0) < 0) {
    error("Error receiving data from the client");
  }
}

void handle_sigint(int sig) {
  static int count = 0;
  count++;
  printf("\nTried to exit %d time(s).\n", count);
  if (count >= 3) {
    printf("Exiting server...\n");
    kill(0, SIGKILL); //kill every child of the parent process.
    exit(EXIT_SUCCESS);
  }
}

void handleClient(int client_socket, int server[2], int child[2]) {
  char testing[150] = "Please choose what kind of calculation you want to perform\n1)Factorial\n2)Triangle area\n3)Exit the program\nPlease input your choice: ";
  sendData(client_socket, testing);
 
  while(1){ 

    int answer = getValidChoice(client_socket, server, child);
    choiceMenu(answer, client_socket);

    if ( answer == 1){
      calculateFactorial(client_socket, server, child);
    }
    else if ( answer == 2) {
      findTriangleArea(client_socket, server, child);
    }
    else if ( answer == 3){
      break;
    }
  }
}


int main(int argc, char *argv[]) {
  int server_socket, client_socket, portnumber;
  struct sockaddr_in server_address, client_address;
  socklen_t client_address_len = sizeof(client_address);

  if (argc < 2){
     fprintf(stderr, "No port provided. Usage %s portnumber\n", argv[0]);
     exit(1);
  }

  // Create socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    error("Error opening socket");
  }

  portnumber = atoi(argv[1]);

  // Specify address
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY; 
  server_address.sin_port = htons(portnumber);

  // Bind socket to address
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    error("Error binding socket");
  }

  // Listen for connections
  listen(server_socket, 5);
  printf("Server listening on port %d...\n", portnumber);

  while(1){

    // Accept incoming connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket < 0) {
       error("Error accepting connection");
    }

    int serverpipe[2];
    int childpipe[2];

    if (pipe(serverpipe) == -1 || pipe(childpipe) == -1) {
       error("Error creating pipe");
    }

     // Create a child process to handle the client
     pid_t pid = fork();
     if (pid < 0) {
       error("Error forking process");
     } 
     else if (pid == 0) {
       // Child process
       signal(SIGINT, SIG_DFL);
       close(server_socket); // Close server socket in child process
       handleClient(client_socket, serverpipe, childpipe);
       close(childpipe[1]);
       close(serverpipe[0]);
       exit(0); 
     }
    else {
      // Parent process
      signal(SIGINT, handle_sigint); 
      close(childpipe[1]);  
      close(serverpipe[0]); 

    // Fork the parent process again to handle the infinite loop of calculations
    pid_t pid2 = fork();

    if (pid2 == -1) {
        error("Error forking process");
    }
    else if (pid2 == 0) {
      // Inside the child process make it so the client can do unlimited amount of calculations
      signal(SIGINT, SIG_DFL);
      while (1) {
        int answer;
        read(childpipe[0], &answer, sizeof(answer));

        if (answer == 1) {
           int number;
           read(childpipe[0], &number, sizeof(number));
           unsigned int result = factorial(number);
           write(serverpipe[1], &result, sizeof(result));
        } 
        else if (answer == 2) {
           int sides[3];
           read(childpipe[0], &sides, sizeof(sides));
           typeOfTriangle(client_socket, serverpipe, childpipe, sides);
        }
        else {
          break;
        }
      }

    close(client_socket);
    close(childpipe[0]);
    close(serverpipe[1]);
    exit(EXIT_SUCCESS);
  }
    // Close client socket after breaking out of the loop
    close(client_socket);
  }
}
  close(server_socket);

  return 0;
}

int getValidChoice(int client_socket, int server[2], int child[2]){
  int answer;
  char client[10];

  bzero(client, sizeof(client));
  bzero(&answer, sizeof(answer));
  readData(client_socket, client, sizeof(client));

  answer = atoi(client);

  while (answer < 1 || answer > 3){
    sendData(client_socket, "Incorrect choice\n");
    bzero(client, sizeof(client));
    readData(client_socket, client, sizeof(client));
    answer = atoi(client);
  }

  printf("Client chose option: %d\n", answer);
  
  close(child[0]);
  write(child[1], &answer, sizeof(answer));

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
     sendData(client_socket, "Exiting...\n");
     break;
  }
}

void calculateFactorial(int client_socket, int server[2], int child[2]){
  unsigned int answer;
  char question[100] = "Input the number of the factorial you want: ";
  char client[100];
  char error[100] = "Error input the number 1 or above\n";
  int number;

  sendData(client_socket, question);

  while (1) {
    readData(client_socket, client, sizeof(client));
    printf("Client: wants the factorial of %s\n", client);

    if (!containsOnlyNumbers(client)) {
      bzero(client, sizeof(client));
      sendData(client_socket, error);
      continue;
    }

    number = atoi(client);
    if (number < 0 || number > 12) {
      bzero(client, sizeof(client));
      sendData(client_socket, error);
      continue;
    }

     close(child[0]);
     write(child[1], &number, sizeof(number));
     close(server[1]);
     sleep(1);
     read(server[0], &answer, sizeof(answer));
     char str[100];
     sprintf(str, "%u", answer);
     sendData(client_socket, str);
     break; // Exit the loop if valid input received
  }
  bzero(question, sizeof(question));
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

void findTriangleArea(int client_socket, int server[2], int child[2]) {
  char choose1[50] = "Please input the first side length: ";
  char choose2[50] = "Please input the second side length: ";
  char choose3[50] = "Please input the third side length: ";
  char client[100];
  char error[100] = "Error input a number 1 or above: ";
  int sides[3];
  int number;
  int i = 0;
  sendData(client_socket, choose1);
  bzero(client, sizeof(client)); 

  while (i < 3) {
    readData(client_socket, client, sizeof(client));
    printf("Client: Side %d: %s\n", i + 1, client);

    if (!containsOnlyNumbers(client)) {
      sendData(client_socket, error);
      bzero(client, sizeof(client));
      continue;
    }

    number = atoi(client);
    if (number <= 0 || number > 100) {
       sendData(client_socket, error);
       bzero(client, sizeof(client));
       continue;
    }

    sides[i] = number;

    if (i == 0) {
      sendData(client_socket, choose2);
    }
    else if (i == 1) {
      sendData(client_socket, choose3);
    }

    i++;
  }

  close(child[0]);
  write(child[1], &sides, sizeof(sides));

  char areaStr[50];
  close(server[1]);
  sleep(1);
  read(server[0], &areaStr, sizeof(areaStr));
  sendData(client_socket, areaStr);

  char triangleType[50];
  sleep(1);
  read(server[0], &triangleType, sizeof(triangleType));
  sendData(client_socket, triangleType);
  bzero(sides, sizeof(sides));
}

void typeOfTriangle(int client_socket, int server[2], int child[2], int sides[3]){
  char equilateral[50] = "The triangle is an equilateral one\n";
  char isosceles [50] = "The triangle is an isosceles one\n";
  char scalene[50] = "The triangle is a scalene one\n";

  int a = sides[0], b = sides[1], c = sides[2];
  double s = (a + b + c) / 2.0;
  double area = sqrt(s * (s - a) * (s - b) * (s - c));

  char areaStr[50];
  sprintf(areaStr, "Area of the triangle: %.2f\n", area);
  close(server[0]);
  write(server[1], areaStr, sizeof(areaStr));

  if ( sides[0] == sides[1] && sides[1] == sides[2]){
    close(server[0]);
    write(server[1], equilateral, sizeof(equilateral));
  }
  else if (sides[0] == sides[1] || sides[0] == sides[2] || sides[1] == sides[2]){
    close(server[0]);
    write(server[1], isosceles, sizeof(isosceles));
  }
  else{ 
    close(server[0]);
    write(server[1], scalene, sizeof(scalene));
  }
}
