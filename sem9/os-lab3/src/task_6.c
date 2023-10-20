#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


struct task_node {
    bool executed;
    int dependencies_number;
    struct task_node** dependencies;
    void (*executable)(void *arg);
};

typedef struct task_node task_node_t;

typedef struct {
    task_node_t* node_list;
    int node_number;
} graph;


void *supporter(void *arg) {
    int timer = rand() % 3000;
    pthread_t tid = pthread_self();
    printf ("Thread %x: going to sleep for %d milliseconds...\n", (unsigned int) tid, timer);
    usleep(timer * 1000);
    printf ("Thread %x: finished!\n", (unsigned int) tid);
    return (void *) tid;
}


graph* create_graph() {
    graph* root = (graph*) malloc(sizeof(graph));
    root->node_number = 10;
    root->node_list = (task_node_t*) malloc(root->node_number * sizeof(task_node_t));

    root->node_list[0].executed = false;
    root->node_list[0].dependencies_number = 0;
    root->node_list[0].executable = &supporter;

    root->node_list[1].executed = false;
    root->node_list[1].dependencies_number = 1;
    root->node_list[1].executable = &supporter;
    root->node_list[1].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 1);
    root->node_list[1].dependencies[0] = &root->node_list[0];

    root->node_list[2].executed = false;
    root->node_list[2].dependencies_number = 1;
    root->node_list[2].executable = &supporter;
    root->node_list[2].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 1);
    root->node_list[2].dependencies[0] = &root->node_list[0];

    root->node_list[3].executed = false;
    root->node_list[3].dependencies_number = 1;
    root->node_list[3].executable = &supporter;
    root->node_list[3].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 1);
    root->node_list[3].dependencies[0] = &root->node_list[2];

    root->node_list[4].executed = false;
    root->node_list[4].dependencies_number = 1;
    root->node_list[4].executable = &supporter;
    root->node_list[4].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 1);
    root->node_list[4].dependencies[0] = &root->node_list[2];

    root->node_list[5].executed = false;
    root->node_list[5].dependencies_number = 1;
    root->node_list[5].executable = &supporter;
    root->node_list[5].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 1);
    root->node_list[5].dependencies[0] = &root->node_list[2];

    root->node_list[8].executed = false;
    root->node_list[8].dependencies_number = 1;
    root->node_list[8].executable = &supporter;
    root->node_list[8].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 1);
    root->node_list[8].dependencies[0] = &root->node_list[5];

    root->node_list[7].executed = false;
    root->node_list[7].dependencies_number = 2;
    root->node_list[7].executable = &supporter;
    root->node_list[7].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 2);
    root->node_list[7].dependencies[0] = &root->node_list[4];
    root->node_list[7].dependencies[1] = &root->node_list[8];

    root->node_list[6].executed = false;
    root->node_list[6].dependencies_number = 3;
    root->node_list[6].executable = &supporter;
    root->node_list[6].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 3);
    root->node_list[6].dependencies[0] = &root->node_list[1];
    root->node_list[6].dependencies[1] = &root->node_list[3];
    root->node_list[6].dependencies[2] = &root->node_list[7];

    root->node_list[9].executed = false;
    root->node_list[9].dependencies_number = 1;
    root->node_list[9].executable = &supporter;
    root->node_list[9].dependencies = (task_node_t**) malloc(sizeof(task_node_t*) * 1);
    root->node_list[9].dependencies[0] = &root->node_list[6];

    return root;
}

void free_graph(graph* gr) {
    for (int i = 0; i < gr->node_number; i++)
        if (gr->node_list[i].dependencies_number > 0)
            free(gr->node_list[i].dependencies);
    free(gr->node_list);
    free(gr);
}


int main() {
    srand(time(NULL));
    graph* root = create_graph();

    bool* nodes_ready = (bool*) malloc(root->node_number * sizeof(bool));
    bool all_nodes_executed = false;
    while (!all_nodes_executed) {

        for (int i = 0; i < root->node_number; i++) {
            bool satisfied = true;
            for (int j = 0; j < root->node_list[i].dependencies_number; j++)
                satisfied &= root->node_list[i].dependencies[j]->executed;
            nodes_ready[i] = satisfied;
        }
        

    }

    free_graph(root);
    return EXIT_SUCCESS;

    for (int i = 0; i < root->node_number; i++) {
        if (root->node_list[i].dependencies_number > 0) {
            bool satisfied = true;
            for (int j = 0; j < root->node_list[i].dependencies_number; j++)
                satisfied &= root->node_list[i].dependencies[j]->executed;
            if (!satisfied) continue;
        }

    }
    

  pthread_t *tids = malloc (nb_threads*sizeof(pthread_t)) ;

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