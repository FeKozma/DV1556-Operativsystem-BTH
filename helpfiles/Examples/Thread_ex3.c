/* Each thread calulates the square of a number and returns it to main()
   Nr of threads excl. main() is given as an argument to the program
  */

#include <stdio.h>   // printf()
#include <stdlib.h>  // atoi(), exit(), ...
#include <pthread.h> // pthread types and functions

void* child(void* arg) 
{ 
  unsigned int* childID = (int*)arg;
  unsigned int *idSquared = malloc(sizeof(unsigned int)); 
  *idSquared = *childID * *childID;
  printf("Child #%u computed %u\n", 
           *childID, *idSquared);
  pthread_exit((void*)idSquared);
}

unsigned int processCommandLine(int argc, char** argv) 
{
  unsigned int retValue = 0;
  if (argc > 1) 
    retValue =  atoi(argv[1]);
  return retValue;
}

int main(int argc, char** argv) 
{
  pthread_t* children;                   // dynamic array of child threads               
  unsigned int sumSquares = 0;
  void* childSquare = NULL;
                                          // get desired # of threads
  unsigned int numThreads = processCommandLine(argc, argv);
                                          // allocate array of handles
  children = malloc( numThreads * sizeof(pthread_t) );
  unsigned int* args = malloc(numThreads * sizeof(unsigned int));
                                          // FORK:
  for (unsigned int id = 0; id < numThreads; id++)
  {
    args[id] = id + 1;
    pthread_create( &(children[id]),   // our handle for the child 
                      NULL,               // attributes of the child
                      child,              // the function it should run
                      (void*)&args[id]);  // args to that function
  }
  
  printf("The parent (main) thread computed %u.\n", sumSquares);

  for (unsigned int id = 0; id < numThreads; id++) 
  {   
    pthread_join(children[id], &childSquare);
    sumSquares += *(int*)childSquare;
  }

  printf("\nThe sum of the squares from  0 to %u is %u.\n\n", numThreads, sumSquares);
  
  free(children);     
  free(childSquare);  
  free (args);                
  return 0;
}

