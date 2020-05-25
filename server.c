/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "part1.c"



void* part3(void* p) {
    while (1) {
        char answer[100];
        scanf ("%s", answer);
        if (strcmp(answer, "q") == 0) {
            pthread_mutex_lock (&lockStop);
            stop_flag = 1;
            pthread_mutex_unlock (&lockStop);
            break;
        }
    }
    return 0;
}

int start_server(int PORT_NUMBER)
{

      // structs to represent the server and client
      struct sockaddr_in server_addr,client_addr;    
      
      int sock; // socket descriptor

      // 1. socket: creates a socket descriptor that you later use to make other system calls
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Socket");
	exit(1);
      }
      int temp;
      if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
	perror("Setsockopt");
	exit(1);
      }

      // configure the server
      server_addr.sin_port = htons(PORT_NUMBER); // specify port number
      server_addr.sin_family = AF_INET;         
      server_addr.sin_addr.s_addr = INADDR_ANY; 
      bzero(&(server_addr.sin_zero),8); 
      
      // 2. bind: use the socket and associate it with the port number
      if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	perror("Unable to bind");
	exit(1);
      }

      // 3. listen: indicates that we want to listen to the port to which we bound; second arg is number of allowed connections
      if (listen(sock, 1) == -1) {
	perror("Listen");
	exit(1);
      }
          
      // once you get here, the server is set up and about to start listening
      printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
      fflush(stdout);
     
    int count = 0; // count the number of pages requested (for debugging purposes)
    
    while(1) { // keep looping and accept additional incoming connections
        
      // 4. accept: wait here until we get a connection on that port
      int sin_size = sizeof(struct sockaddr_in);
      int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);

      if (fd != -1) {
         printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

        // buffer to read data into
        char request[1024];

        // 5. recv: read incoming message (request) into buffer
        int bytes_received = recv(fd,request,1024,0);
        // null-terminate the string
        request[bytes_received] = '\0';
        // print it to standard out
        printf("REQUEST:\n%s\n", request);

        count++; // increment counter for debugging purposes

        // this is the message that we'll send back
        char* response = (char*)malloc(3000 * sizeof(char)); // char* response = (char*)malloc(100 * sizeof(char));
          pthread_mutex_lock (&lockCurr); 
          pthread_mutex_lock (&lockMax);
          pthread_mutex_lock (&lockAvg);
        
        sprintf(response, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><input id=\"thre\"></input><button id=\"update\">update</button><p>It works!<br>count=<span id=\"count\">%d</span><br>Threshold you've set (in %%): <span id=\"th\"> </span><br><span id=\"alert\">Latest CPU Usage: <span id=\"current\">%.2f</span>%%</span><br>Max CPU Usage: %.2f%%<br>Avg CPU Usage: %.2f%%</p><script>          var timesVisited = 0;     if(localStorage.timesVisited ){timesVisited=parseFloat(localStorage.timesVisited);} localStorage.setItem('timesVisited', timesVisited);   var report=document.getElementById('th'); report.innerHTML=timesVisited; f(1); h();                     var clicks = 0;  function f(e) { var input = document.getElementById('thre');var field = document.getElementById('th'); field.innerHTML = input.value;   clicks = 0; } function h() {var c = document.getElementById('current').value=document.getElementById('current').innerHTML;  var t = document.getElementById('th').value=document.getElementById('th').innerHTML;  c=parseFloat(c);t=parseFloat(t);              if (c >= t){ document.getElementById('alert').style.color='red';  alert(\"Warning: Current CPU has exceeded the threshold!\");  } if (c < t){document.getElementById('alert').style.color='black';}       localStorage.setItem('timesVisited', document.getElementById('th').value);        }   function g() {clicks++}; var button=document.getElementById('update');button.addEventListener('click', g);  document.addEventListener('click', f);             </script></html>", count, curr, max, avg);
        
          
          
          pthread_mutex_unlock (&lockCurr); 
          pthread_mutex_unlock (&lockMax);
          pthread_mutex_unlock (&lockAvg);
        printf("RESPONSE:\n%s\n", response);


          
        // 6. send: send the outgoing message (response) over the socket
        // note that the second argument is a char*, and the third is the number of chars	
        send(fd, response, strlen(response), 0);

          free(response);

        // 7. close: close the connection
        close(fd);
        printf("Server closed connection\n");
          
        ///
        ///
        pthread_mutex_lock (&lockStop);
        if (stop_flag == 1) {
            pthread_mutex_unlock (&lockStop);
            break;
        }
        pthread_mutex_unlock (&lockStop);
        ///
        ///
          
       }
    }

    // 8. close: close the socket
    close(sock);
    printf("Server shutting down\n");
  
    return 0;
} 



int main(int argc, char *argv[])
{
    /*
  // check the number of arguments
  if (argc != 2) {
      printf("\nUsage: %s [port_number]\n", argv[0]);
      exit(-1);
  }

  int port_number = atoi(argv[1]);
  if (port_number <= 1024) {
    printf("\nPlease specify a port number greater than 1024\n");
    exit(-1);
  }*/
    pthread_mutex_init(&lockCurr, NULL);
    pthread_mutex_init(&lockMax, NULL);
    pthread_mutex_init(&lockAvg, NULL);
    
    pthread_t t1;
    int ret = pthread_create(&t1, NULL, part1, NULL);
    // now, part1 | main
    if (ret != 0) return -1;
    
    pthread_t t2;
    int rett = pthread_create(&t2, NULL, part3, NULL);
    // now, part3 | main
    if (rett != 0) return -1;
    
    int port_number = 3000; // hard-coded for use on Codio
    start_server(port_number);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    //gcc -o test test.c -pthread
}

