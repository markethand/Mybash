#include "exec.h"

#define ARR_PID_LENGTH 100

char CD_STRING[] = "cd";

pid_t * BACKGRND_PIDS = NULL;
int   BACK_PID_LENGTH = 0;
pid_t * FOREGRND_PIDS = NULL;
int   FORE_PID_LENGTH = 0;
pid_t * AUXILARY_PID = NULL;


int WAIT_Q = 0; //Кол-во процессов обрабатываемых waitом

void SigHndlr () /* обработчик сигнала */
  {
  //Мы будем использовать этот обработчким
  //Для обработка SIGCHLD
  //Если такой сигнал пришел, мы будем смотреть
  //На все фоновые процессы.
  //int dead_id = 0;

  //int zombie_id = -1;
  int zombie_flag = 1; //продолжаем пока есть зомби

  //printf ("BACKHANDLER\n");
  while (zombie_flag)
    {
    //printf ("BACKHANDLER_CYCLE\n");

    zombie_flag = 0;

    for (int i = 0; i < BACK_PID_LENGTH; i++)
      {
      //if (BACKGRND_PIDS[i] != 0)
      //printf ("__read: [%d]= %d \n", i, *(BACKGRND_PIDS + i));
      //printf ("address = %d\n", BACKGRND_PIDS);

      if (*(BACKGRND_PIDS + i) != 0)
        { //Это был какой то фоновый процесс
        //printf ("BCKGRND PID [%d]= %d \n", i, *(BACKGRND_PIDS + i));

        //printf ("__read: [%d]= %d \n", i, *(BACKGRND_PIDS + i));
        if (*(BACKGRND_PIDS + i) == waitpid (*(BACKGRND_PIDS + i), NULL, WNOHANG))
          {
          //Помер один из фоновых
          printf ("[%d] stopped \n", *(BACKGRND_PIDS + i));
          *(BACKGRND_PIDS + i) = 0;
          zombie_flag = 1;
          }
        }
      }
    }

  //printf ("BACKHANDLER ENDED\n");
  }




void add_back_proc (int this_pid)
  {
  printf ("ABP\n");
  for (int i = 0; i <  BACK_PID_LENGTH; i++)
    {
    if (BACKGRND_PIDS[i] == 0)
      {
      BACKGRND_PIDS[i] = this_pid;
      printf ("[%d] added to background \n", BACKGRND_PIDS[i]);
      return;
      }
    }

  printf ("add_back_proc\n");

  //Если мы дошли до сюда, значит нужна реаллокация памяти.
  //printf ("unable to add process");

  AUXILARY_PID = calloc(BACK_PID_LENGTH + ARR_PID_LENGTH, sizeof (pid_t));
  for (int i = 0; i < BACK_PID_LENGTH; i++) AUXILARY_PID [i] = BACKGRND_PIDS [i];
  free (BACKGRND_PIDS);
  BACKGRND_PIDS = AUXILARY_PID;
  BACK_PID_LENGTH += ARR_PID_LENGTH;
  AUXILARY_PID = NULL;

  BACKGRND_PIDS[BACK_PID_LENGTH - ARR_PID_LENGTH] = this_pid;

  return;
  }

void add_fore_proc (int this_pid)
  {
  printf ("AFP\n");

  for (int i = 0; i < FORE_PID_LENGTH; i++)
    {
    if (FOREGRND_PIDS[i] == 0)
      {
      FOREGRND_PIDS[i] = this_pid;
      printf ("[%d] added to foreground \n", FOREGRND_PIDS[i]);
      return;
      }
    }

  printf ("add_fore_proc\n");

  AUXILARY_PID = calloc(FORE_PID_LENGTH + ARR_PID_LENGTH,  sizeof (pid_t));
  for (int i = 0; i < FORE_PID_LENGTH; i++) AUXILARY_PID [i] = FOREGRND_PIDS [i];
  free (FOREGRND_PIDS);
  FOREGRND_PIDS = AUXILARY_PID;
  FORE_PID_LENGTH += ARR_PID_LENGTH;
  AUXILARY_PID = NULL;

  FOREGRND_PIDS[FORE_PID_LENGTH - ARR_PID_LENGTH] = this_pid;

  return;
  }


//Возвращает глубину конвейера
int give_tree_depth (tree this_tree)
  {
  int i = 0;
  while (this_tree != NULL)
    {
    this_tree = this_tree -> pipe;
    i++;
    }
  return i;
  }



void init_proc_arrays ()
  {
  BACKGRND_PIDS = calloc (ARR_PID_LENGTH, sizeof (pid_t));
  BACK_PID_LENGTH = ARR_PID_LENGTH;
  FOREGRND_PIDS = calloc (ARR_PID_LENGTH, sizeof (pid_t));
  FORE_PID_LENGTH = ARR_PID_LENGTH;
  }

void free_proc_arrays ()
  {
  free(BACKGRND_PIDS);
  free(FOREGRND_PIDS);
  }

int executor_main (  tree this_tree)
  {
  //SigHndlr();
  //return 0;

  //printf ("ADDR = %ld \n", BACKGRND_PIDS);

  int fd[2], in, oldin, out, i, tmp_fd1, tmp_fd2, this_pid;
  int tmp_length; int counter = 0;


  //printf ("DEBUG\n");
  //SigHndlr();

  tree comma_tree = this_tree;
    //СТАВИМ ОБРАБОТЧИК ДЛЯ ФОНОВЫХ ПРОЦЕССОВ
    signal (SIGCHLD, SigHndlr);
    //Игнорируем SIGPIPE???????????????????????????????????????????????????????????????????????????????????????
    signal (SIGPIPE, SIG_IGN);

    //ВНЕШНИЙ ОБХОД ПО ДРЕВОВИДНОЙ СТРУКТУРЕ - по NEXTам.
    for (; comma_tree != NULL; comma_tree = comma_tree -> next)
      {
        this_tree = comma_tree;
        tmp_length = give_tree_depth (this_tree); //глубина конвейера

        WAIT_Q = tmp_length;


        if (tmp_length == 0) continue;
        if (tmp_length == 1)
            {
            //printf ("DEBUG1\n");
            //SigHndlr();


            //Если это cd то не нужно делать fork и exec

            if (strcmp((this_tree -> argv)[0], CD_STRING) == 0)
              {
              //-----------------СМЕНА ДИРЕКТОРИИ с помощью CD------------
              if ((this_tree -> argv)[1] == NULL)
                {
                if (chdir (getenv("HOME"))) goto cd_error;
                continue;
                }
              else
                {
                if (chdir ( (this_tree -> argv)[1] )) goto cd_error;
                continue;
                }
              }
            else
              {
                if ((this_pid = fork ()) == 0)
                  {
                  if (this_tree -> infile != NULL)
                    {
                    tmp_fd1 = open(this_tree -> infile, O_RDONLY);
                    if (tmp_fd1 == -1) goto  no_file_error;
                    dup2 (tmp_fd1, 0);
                    close (tmp_fd1);
                    }

                  if (this_tree -> outfile != NULL)
                    {
                    if (this_tree -> append)
                      {
                      tmp_fd2 = open(this_tree -> outfile, O_WRONLY | O_APPEND);
                      if (tmp_fd2 == -1) goto  no_file_error;
                      dup2 (tmp_fd2, 1);
                      close (tmp_fd2);
                      }
                    else
                      {
                      tmp_fd2 = open(this_tree -> outfile, O_WRONLY | O_CREAT );
                      if (tmp_fd2 == -1) goto  no_file_error;
                      dup2 (tmp_fd2, 1);
                      close (tmp_fd2);
                      }
                    }
                  execvp ( (this_tree -> argv)[0], this_tree -> argv);

                  //Если мы здесь, значит execvp не выполнился.
                  //Прежде чем переходить к ошибке, нужно закрыть каналы.
                  //А у нас открыты только стандартные
                  goto exec_error;
                  }
                if (this_tree -> backgrnd)
                  {
                  add_back_proc (this_pid);

                  //printf ("DEBUG2\n");
                  //SigHndlr();
                  }
                else
                  { //Вырожденный случай wait
                  waitpid (this_pid, NULL, 0);
                  }
              }

            }
        else
          {
            //================= ============== tmp_length >= 2  ============ ==================

            pipe (fd);
            in = fd[0]; //чтение
            out = fd[1]; //запись


            //Если тут cd, то обходимся без fork

            if (strcmp((this_tree -> argv)[0], CD_STRING) == 0)
              {
              WAIT_Q--;//Игнорирование cd;
              }
            else
                {
                /*Мы в процессе - сыне (№1 в конвейере)
                В этой связи мы приравниваем вывод в канал
                к стандартному выводу и закрываем out
                т.к. для вывода в канал есть stdout  */
                if ((this_pid = fork ()) == 0)
                  {
                  //Если есть ввод из файла, то применяем его.
                  if (this_tree -> infile != NULL)
                    {
                    tmp_fd1 = open(this_tree -> infile, O_RDONLY);
                    if (tmp_fd1 == -1) goto  no_file_error;
                    dup2 (tmp_fd1, 0);
                    }
                  dup2 (out, 1);
                  close (out);
                  //Не забываем и про закрытие ввода внутри сына
                  //Или сын будет и писателем и читателем этого канала
                  //А значит, процесс будет подвисать.
                  close (in);
                  execvp ( (this_tree -> argv)[0], this_tree -> argv);

                  //Если мы здесь, значит execvp не выполнился.
                  //
                  goto exec_error;
                  }
                if (this_tree -> backgrnd)
                  {
                  add_back_proc (this_pid);
                  WAIT_Q--;
                  }
                else
                  {
                  add_fore_proc (this_pid);
                  }
                }




            /* Мы в родителе.
            Для того чтобы читать у нас есть старый in,
            его мы должны будем проассоциировать с stdin.

            А для передачи данных следующему,
            нам понадобится новый канал */

            //printf ("===========CYC %d\n", tmp_length);
            for (i = 2; i < tmp_length; i++)
              {
              //Заходим в следующую структуру по конвейеру
              this_tree = this_tree -> pipe;

              //Закрываем канал для записи (в него вводить будет предыдущая итерация)
              close (out);


              //И создаем новый канал, между этой и следующей итерацией
              //Чтобы читать у нас есть oldin
              oldin = in;

              //А для нового канала есть in и out
              pipe (fd);
              in = fd[0];
              out = fd[1];

              if (strcmp((this_tree -> argv)[0], CD_STRING) == 0)
                {
                WAIT_Q--;
                //printf ("cd ignored cyc\n"); //Игнорирование;//Игнорирование cd;
                }
              else
                  {
                  //printf ("---------------------debug: in = %d | out = %d\n", oldin, out);

                  if ((this_pid = fork ()) == 0)
                    { //Будем считать что файлов у нас здесь нет
                      //А если есть то они игнорируются
                    dup2 (oldin, 0);
                    close (oldin);
                    dup2 (out, 1);
                    close (out);

                    //Не забываем и про закрытие in
                    //или сын будет и читателем и писателем в новый канал
                    close (in);

                    execvp ( (this_tree -> argv)[0], this_tree -> argv);

                    //Если мы здесь, значит execvp не выполнился.
                    goto exec_error;
                    }
                  if (this_tree -> backgrnd)
                    {
                    add_back_proc (this_pid);
                    WAIT_Q--;
                    }
                  else
                    {
                    add_fore_proc (this_pid);
                    }
                  }

              //закрываем старое чтение из нового канала
              //так как у новых каналов новые средства коммуникации
              close(oldin);
              }

            //Закрывам запись - выводить мы будем или в stdout или в файл.
            close(out);




            //Переходим в следующее дерево
            this_tree = this_tree -> pipe;

            //printf ("===========END\n");
            if (strcmp((this_tree -> argv)[0], CD_STRING) == 0)
              {
              WAIT_Q--;//printf ("cd ignored last\n"); //Игнорирование
              }
            else
              {
                //Последний сын
                //printf ("-----------last----debug: in = %d\n" , in);
                if ((this_pid = fork ()) == 0)
                  {
                  dup2(in, 0);
                  close(in);

                  //Смотрим куда мы выводим.
                  if (this_tree -> outfile != NULL)
                    {
                    if (this_tree -> append)
                      {
                      tmp_fd2 = open(this_tree -> outfile, O_WRONLY | O_APPEND);
                      if (tmp_fd2 == -1)
                        {
                        //Нужно озаботиться закрытием канала.
                        close (in);
                        goto  no_file_error;
                        }

                      dup2 (tmp_fd1, 1);
                      }
                    else
                      {
                      tmp_fd2 = open(this_tree -> outfile, O_WRONLY | O_CREAT );
                      if (tmp_fd2 == -1)
                        {
                        //Нужно озаботиться закрытием канала.
                        close (in);
                        goto  no_file_error;
                        }
                      dup2 (tmp_fd2, 1);
                      }
                    }

                  execvp ( (this_tree -> argv)[0], this_tree -> argv);

                  //Если мы здесь, значит execvp не выполнился.
                  goto exec_error;
                  }
                if (this_tree -> backgrnd)
                  {
                  add_back_proc (this_pid);
                  WAIT_Q--;
                  }
                else
                  {
                  add_fore_proc (this_pid);
                  }
              }

            //Закрываем чтение этого канала, читать из него будет только сын
            close(in);

            //printf ("DEBUG; TMP_LENGTH = %d\n", tmp_length);
            //if (comma_tree -> backgrnd == 0) for( i = 1; i <= tmp_length; i++) wait (NULL);
            //printf ("DEBUG 15 11 2020\n");

            counter = 0;

            printf ("-------------debug %d\n", WAIT_Q);
            //fprintf(stderr, "-------------debug %d\n", WAIT_Q);

            while (counter < WAIT_Q)
              {
              for (int i = 0; i < FORE_PID_LENGTH; i++)
                {
                if (FOREGRND_PIDS[i] != 0)
                  { //Это был какой то фоновый процесс
                  if (FOREGRND_PIDS[i] == waitpid (FOREGRND_PIDS[i], NULL, WNOHANG))
                    {
                    //Помер один из переднеплановых
                    //printf ("foreground [%ld] stopped \n", FOREGRND_PIDS[i]);
                    FOREGRND_PIDS[i] = 0;
                    counter++;
                    }
                  }
                }
              }
         }
    }

  return 0;

  no_file_error:
    printf ("error: unable to open the file");
  return 1;;
  cd_error:
    printf ("error: cd error");
  return 1;
  exec_error:
    printf ("error: exec error");
  return 1;
  }


