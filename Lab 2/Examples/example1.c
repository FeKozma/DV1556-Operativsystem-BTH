/* Demonstration of creation nd termintion of threads
 */

#include <stdio.h>   // printf()
#include <pthread.h> // pthread types and functions

void* child() 
{ 
   printf("This is the child thread\n");
   return NULL;
}

int main() 
{
  pthread_t thread;                     // struct for child-thread info
                                       // Spawn child thread:
  pthread_create( &thread,              //  the handle for it
                   NULL,               //  its attributes 
                   child,              //  the function it should run
                   NULL );             //  args to that function
  
  printf("This is the parent (main) thread.\n");

  pthread_join(thread, NULL);         // JOIN: wait for child to finish
  
  return 0;
}

