#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
  char *lyrics;
  int times;
} func_args;

void *supporter (void *arg){
  func_args      *args = (func_args *) arg ;
  int       i ;
  int       pid ;
  pthread_t tid ;  
  pid = getpid () ;
  tid = pthread_self () ;

  for (i = 0; i < args->times; i++){
      printf ("Process %d Thread %x : %s \n", pid, (unsigned int) tid, args->lyrics) ;
  }
  return (void *) tid ;
}

int main (int argc, char **argv){

  int team1, number1 ;
  int team2, number2 ;
  int i ;
  int nb_threads = 0 ;
  pthread_t *tids ;
  func_args team1_args, team2_args ;

  if (argc != 5) {
    fprintf(stderr, "usage : %s team1 number1 team2 number2\n", argv[0]) ;
    exit (-1) ;
  }

  team1 = atoi (argv[1]) ;
  team2  = atoi (argv[3]) ;
  number1 = atoi (argv[2]) ;
  number2  = atoi (argv[4]) ;
  team1_args.lyrics = "Allons enfants de la patrie";
  team1_args.times = number1;
  team2_args.lyrics = "Swing low, sweet chariot";
  team2_args.times = number2;
  nb_threads = team1 + team2; 
  tids = malloc (nb_threads*sizeof(pthread_t)) ;

  /* Create the threads for team1 */
  for (i = 0 ; i < team1; i++){
    pthread_create (&tids[i], NULL, supporter, &team1_args) ;
  }
  /* Create the other threads (ie. team2) */
  for ( ; i < nb_threads; i++){
    pthread_create (&tids[i], NULL, supporter, &team2_args) ;
  }  

  /* Wait until every thread ended */ 
  for (i = 0; i < nb_threads; i++){
    pthread_join (tids[i], NULL) ;
  }
  
  free (tids) ;
  return EXIT_SUCCESS;
}
