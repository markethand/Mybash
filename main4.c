#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include "exec.h"




int main()
  {

  char ** Word_List = NULL;
  //char * aux_cd_str = NULL; //Используется для cd

  tree this_tree = NULL;
  tree comma_tree = NULL;
  tree tree_root = NULL;

  int fd[2], in, oldin, out, i, tmp_fd1, tmp_fd2, this_pid;
  int tmp_length;
  //int BACKGRND_MODE = 0;

  //Пока не достигнут конец ввода

  //Процессы.
  init_proc_arrays ();


  while (!GIVE_END_FLAG())
    {
    //Создается список слов из строки
    //Учитываются кавычки, подстановки, и т д.

    printf ("\n=>");
    Word_List = list_create();

    //printf ("DEBUG");
    this_tree = create_tree (Word_List);

    //if (this_tree != NULL) print_tree (this_tree, 5);
    tree_root = this_tree;



    //Исполняет древовидную структуру
    if (executor_main (tree_root) == 1)
      {//Случилась ошибка
      list_delete (&Word_List);
      if (this_tree != NULL) delete_tree (tree_root);
      break;
      }




    list_delete (&Word_List);
    if (this_tree != NULL) delete_tree (tree_root);

    tree_root = NULL;
    this_tree = NULL;

    //sleep (1);
    }

  free_proc_arrays ();
  }
