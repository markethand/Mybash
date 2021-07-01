#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>



void SigHndlr ();
void add_back_proc (int this_pid);
void add_fore_proc (int this_pid);
int give_tree_depth (tree this_tree);
void init_proc_arrays ();
void free_proc_arrays ();
int executor_main (  tree this_tree);


