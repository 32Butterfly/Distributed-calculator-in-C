# Project about distributed calculator

Evaldas Dmitrišin IT 1st year 3rd group student

## **How the code works** 

1. Clone this repository!
2. In your local machine go into the task3 direcotory
3. Run make to create executable files
4. In the first terminal run ./server
5. In the other terminals run ./client

In this file I will document my successes/failures and general updates about the code.

### 2024/05/03
I played around a bit with sockets and server/client functionality. Have not included a loop so the converstation goes on forever nor have I made any actual functionality but played around
with sending data back and forth between the server and the client! Time spent 1h 30 min

Made a bit of functionality this was more to get used to fully using sending and recieving data from client and server. Overall I think I got how it works even if I do quite a bit of errors
before succeding. Time spent another 1 hour.

### 2024/05/04
Made it so factorial is calculated but it needs some more revision in terms of error handaling.

### 2024/05/06
Made the error handaling for factorial so it should run without any problems I still plan on making more descriptive error for if you have the number too big. Time spent: 1 hour!

I found out you can build the files using Makefile and then just running make! This is really good for considering I have already deleted the server once running gcc -o client.c client
instead of gcc client.c -o client. So this will make sure I never make this mistake again!!! Time spent aroung 40 minutes because of tutorials and implementation.

### 2024/05/07
Made it so the make file uses -lm flag for Makefile so now it can utilize the sqrt function otherwise it would not work. Also made it so the code works for the second option so now it
calculates the triangle area. The error handaling is not working yet and I plan to also make it so it displays what kind of triangle it is.

### 2024/05/08
Played around with the markdown language. Also made error checkings for sending and recieving data. However ultimetely I will change it because there is a lot of redundant code.

### 2024/05/13
Made new functions for recieving and sending data which checks whether or not they were succefull making the code more error proof. I'm currently thinking what the third operation should be
whether it's a simple calculator or something else.

Refactored the code as well as made it so the triangle area calculation also prints what kind of triangle it is based on the lenghts of the sides. Will start trying to implement the forking
Time spent 45 minutes.
