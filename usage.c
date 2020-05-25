#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
int stop_flag = 0;
pthread_mutex_t lockStop;

double CPU_usage = 0; // [0-100(%)]
double CPU_max = 0;
double CPU_avg = 0; // (sum of pool within count) / count

// these variables are just for lockCurr
double curr = 0;
double max = 0;
double avg = 0;

pthread_mutex_t lockCurr, lockMax, lockAvg;

double CPU_pool[3600]; // 60 sec * 60 min = 3600 pc CPU data
int CPU_pool_index = 0; // from 0 to 3599
int CPU_cal_count = 1; // from 1 to 3600
// this amount = (index+1) within 1hr. After 1hr, == 3600.

void* usage(void* p) {  //int main(int argc, char *argv[]) {

    int old = -1;
    int new = -1;
    
    while (1) {
        
        FILE *file;
        file = fopen("/proc/stat","r");

        char line[100];

        if (fgets(line, sizeof(line), file)) {


           char * token = strtok(line, " ");
           // loop through the string to extract all other tokens
           int count = 0;
           while( token != NULL ) {
               pthread_mutex_lock (&lockStop);
               if (stop_flag == 1) {
                   pthread_mutex_unlock (&lockStop);
                   return 0;
               }
               pthread_mutex_unlock (&lockStop);
               if (count == 4) {
                   old = new;
                   new = atoi(token);


                   if (old != -1) CPU_usage = 100 - ((double)(new - old) / 4);
                   if (CPU_usage < 0) {
                       CPU_usage = 0;
                   } else if (CPU_usage > 100) {
                       CPU_usage = 100;
                   } 
                   if (old != -1) {
                       if (CPU_cal_count > 3600) CPU_cal_count = 3600;
                       if (CPU_pool_index > 3599) CPU_pool_index = 0; 
                       
                       if (CPU_max < CPU_usage) CPU_max = CPU_usage;
                       // first update the pool array
                       CPU_pool[CPU_pool_index] = CPU_usage;

                       
                       // then calculate avg by calculate sum
                       double sum = 0;
                       int i = 0;
                       for (i = 0; i < CPU_cal_count; i++) sum += CPU_pool[i];
                       CPU_avg = sum / CPU_cal_count;
                       
                       CPU_pool_index++;
                       CPU_cal_count++;
                       
                       
          pthread_mutex_lock (&lockCurr); 
          pthread_mutex_lock (&lockMax);
          pthread_mutex_lock (&lockAvg);         
                       curr = CPU_usage;
                       max = CPU_max;
                       avg = CPU_avg;
                       printf("Current CPU usage: %.2f %%\n", curr);

          pthread_mutex_unlock (&lockCurr); 
          pthread_mutex_unlock (&lockMax);
          pthread_mutex_unlock (&lockAvg); 
                       //printf("\n");
                   }
                   printf("\n");
                   break;
               }
               token = strtok(NULL, " ");
               count++;
           }    
        }
        fclose(file);   
        sleep(1); // sleep one second
    }
    
    FILE *file;
    file = fopen("/proc/stat","r");
    
    char line[100];
    
    if (fgets(line, sizeof(line), file)) {
        printf("next line= %s\n", line);

       char * token = strtok(line, " ");
       // loop through the string to extract all other tokens
       int count = 0;
       while( token != NULL ) {
           if (count == 4) {
               new = atoi(token);
               printf("New number is %d. Old number is %d.\n", new, old);
               break;
           }
           token = strtok(NULL, " ");
           count++;
       }    
    }
    fclose(file);
    
}