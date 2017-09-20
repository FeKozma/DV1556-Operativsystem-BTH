#include <stdio.h>   
#include <stdlib.h>  
#include <pthread.h> 
#include <sys/time.h>     


struct threadArgs {
     unsigned int  id;
     unsigned int numThreads;
     struct timeval startTime;
};

void* child(void* params) 
{ 
   struct timeval endTime; 
   gettimeofday(&endTime, NULL);
   struct threadArgs *args = (struct threadArgs*) params;
   unsigned int childID = args->id;
   unsigned int  numThreads = args->numThreads;
   struct timeval startTime = args->startTime;
   double creationTime = (endTime.tv_usec - startTime.tv_usec)/1000000.0;
   printf("Greetings from child #%u of %u which took %f secs to create\n", 
           childID, numThreads, creationTime);
   free(args);
}

unsigned int processCmdLine(int argc, char** argv) 
{
    unsigned int retValue = 1;
    if (argc > 1) 
        retValue =  atoi(argv[1]);
    return retValue;
}

int main(int argc, char** argv) 
{
  pthread_t* children;                   // dynamic array of child threads             
  struct threadArgs* args;               // argument buffer
                                          
  unsigned int numThreads = processCmdLine(argc, argv);   // get desired # of threads
                                          
  children = malloc(numThreads * sizeof(pthread_t)); // allocate array of handles
                                          
  for (unsigned int  id = 0; id < numThreads; id++)   // create threads
  {
    args = malloc(sizeof(struct threadArgs));
    args->id = id; 
    args->numThreads = numThreads;
    gettimeofday(&(args->startTime), NULL);
    pthread_create(&(children[id]),   // our handle for the child 
                      NULL,               // attributes of the child
                      child,     // the function it should run
                      (void*)args);     // args to that function
  }  
  printf("I am the parent (main) thread.\n");

  for (unsigned int id = 0; id < numThreads; id++) 
    pthread_join(children[id], NULL );
  
  free(children);                         // deallocate array
  return 0;
}