#include "tree.h"


tree create_empty_node (void)
  {
  // Непусто, мы выделяем память
  tree this_tree = malloc (1 * sizeof (node));
  // Не забываем теперь про обнуление переменных

  //Создаем массив 10 элементов указателей на char
  //                     (см. описание)
  this_tree -> argv = calloc (10 * sizeof (char*), 1);
  this_tree -> infile = NULL;
  this_tree -> outfile = NULL;
  this_tree -> append = 0;
  this_tree -> backgrnd = 0;
  this_tree -> psubcmd = NULL;
  this_tree -> pipe = NULL;
  this_tree -> next = NULL;
  this_tree -> type = NXT;

  return this_tree;
  }


void set_tree_backgrnd_1 (tree this_tree)
  {
  if (this_tree == NULL) return;
  else
    {
    this_tree -> backgrnd = 1;
    set_tree_backgrnd_1(this_tree -> pipe);
    }
  }


void make_shift(int n){
    while(n--)
        putc(' ', stderr);
}

void print_argv(char **p, int shift){
    char **q=p;
    if(p!=NULL){
        while(*p!=NULL){
             make_shift(shift);
             fprintf(stderr, "argv[%d]=%s\n",(int) (p-q), *p);
             p++;
        }
    }
}

void print_tree(tree t, int shift){
    char **p;
    if(t==NULL)
        return;
    p=t->argv;
    if(p!=NULL)
        print_argv(p, shift);
    else{
        make_shift(shift);
        fprintf(stderr, "psubshell\n");
    }
    make_shift(shift);
    if(t->infile==NULL)
        fprintf(stderr, "infile=NULL\n");
    else
        fprintf(stderr, "infile=%s\n", t->infile);
    make_shift(shift);
    if(t->outfile==NULL)
        fprintf(stderr, "outfile=NULL\n");
    else
        fprintf(stderr, "outfile=%s\n", t->outfile);
    make_shift(shift);
    fprintf(stderr, "append=%d\n", t->append);
    make_shift(shift);
    fprintf(stderr, "background=%d\n", t->backgrnd);
    make_shift(shift);
    fprintf(stderr, "type=%s\n", t->type==NXT?"NXT": t->type==OR?"OR": "AND" );
    make_shift(shift);
    if(t->psubcmd==NULL)
        fprintf(stderr, "psubcmd=NULL \n");
    else{
        fprintf(stderr, "psubcmd---> \n");
        print_tree(t->psubcmd, shift+5);
    }
    make_shift(shift);
    if(t->pipe==NULL)
        fprintf(stderr, "pipe=NULL \n");
    else{
        fprintf(stderr, "pipe---> \n");
        print_tree(t->pipe, shift+5);
    }
    make_shift(shift);
    if(t->next==NULL)
        fprintf(stderr, "next=NULL \n");
    else{
        fprintf(stderr, "next---> \n");
        print_tree(t->next, shift+5);
    }
}





//Count Chars In String (not including zero)
int CCIST (char * str)
  {
  int i = 0;

  while (*(str + i) != '\0') i++;

  return i;
  }

enum STR_CLASSIFIER classificator (char *this_string)
  {
  int this_str_len = 0;
  char this_str_first_char = 0;
  this_str_len = CCIST(this_string);
  if (this_string == NULL) return ERROR;

  this_str_first_char = this_string[0];

  if (this_str_len == 1)
    {
    switch (this_str_first_char)
      {
      case '|':
        return PIPE; break;

      case '>':
        return BIGGER; break;

      case '<':
        return SMALLER; break;

      case '&':
        return AMPERSANT; break;

      case ';':
        return COMMADOT;  break;

      default:
        return FILE_OR_COMMAND;  break;
      };
    }
  else if (this_str_len == 2)
    {
    switch (this_str_first_char)
      {
      case '|':
      case '<':
      case '&':
      case ';':
        return ERROR;
        break;
      default:
        ;  break;
      };

    if (this_string[1] == '>') return DOUBLEBIGGER;
    else return FILE_OR_COMMAND;
    }
  else
    {
    return FILE_OR_COMMAND;
    }
  }

tree create_tree (char **Word_List)
  {
  enum LGRAPH_STATE tmp_state = BEGIN;
  int i = 0, j = 0 , k = 0;

    //=================ПОСТРОЕНИЕ ДЕРЕВА ПО СПИСКУ ЛЕКСЕМ ==============
  //Замечание: важно понимать, что память я н еперевыделял

  //Создадим tree и node
  tree this_tree = NULL; // Дерево может быть и пустым, начало - пустое
  tree tree_root = NULL; // На случай если пригодится хранить корень дерева
  tree prev_tree = NULL; // предыдущая древовидная структура
  tree pointcomma_tree = NULL; // последняя по верхнему уровню (точкам с запятыми)

  //Создадим переменную для классификации строк
  enum STR_CLASSIFIER this_class;



  tmp_state = BEGIN;
  i = 0; // Индекс чтобы двигаться по списку из строк


  //Пока слова в данной строке не закончились,
  //будем считывать их по одному учитывая состояния.
  while (tmp_state != END)
    {
    //printf ("11112020_CYCLE\n");
    //i - напомню, индекс в массиве ссылок настроки
    switch (tmp_state)
      {
      case BEGIN:
        //printf ("11112020_BEGIN\n");
        //Начальное состояние, i = 0;
        /*Случаев у нас 2:
          1. Word_List пуст
            тогда завершить построение дерева.
          2. Word_List непуст
            тогда создать узел с соответственно названием команды
            и  если это команда изменить состояние  на CONV */
        if (*(Word_List + i) == NULL) tmp_state = END;
        else
          {
          //Эта ячейка не пуста, теперь нужно скопировать туда имя команды, если это команда
            //__________________       ЗАМЕЧАНИЕ       ______________________
            // Чтобы понимать, чем является аргумент, мы воспользуемся функцией classificator
            // Если это файл или команда (просто ИМЯ а не иное) то ОК


          if (classificator (*(Word_List + i)) == FILE_OR_COMMAND)
            {
            //Вызываем функцию создания ячейки.
            this_tree = create_empty_node();
            tree_root = this_tree;
            pointcomma_tree = this_tree;


            //А именно скопировать всё что нужно в  argv.

            //Выделяем по длине строки(не включая 0) и 1 символ для \0
            /*(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
            j = 0;
            while(*(Word_List + i)[j] != 0)
              {
              (this_tree -> argv)[0][j]  = *(Word_List + i)[j];
              }
            (this_tree -> argv)[0][j] = 0;*/


            (this_tree -> argv)[0] = *(Word_List + i);

            //Таким образом мы создали ячейку древовидной структуры
            //и вписали название. Теперь состояние CONV

            tmp_state = CONV;
            }
          else
            {
            goto error_syntax;
            }
          }

      break;


      case CONV:
        //printf ("11112020_CONV\n");
        //Состояние нахождения в конвейере
                 // а именно считывание файлов, аргументов, и т д
        //Прежде всего проверяем, пустота или не пустота.
        //И если пустота, заканчиваем ввод

        if (*(Word_List + i) == NULL)
          {
          tmp_state = END;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //Смотрим какая это строка
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //printf ("FILE_OR_COMMAND:%s \n", *(Word_List + i));

              //Это какое то имя и мы воспринимаем его как аргумент
              //Тогда нам нужно добавить аргумент
              //Как нам это сделать?
              //Нам нужно отойти в массиве argv до NULL,

              //выражение корректно т.к. сначала считается левое значение.
              for (k = 0; (k < 10)&&( (this_tree -> argv)[k] != NULL); k++);                                                 //------------------------------------------------ЧИСЛО 10
              if (k == 10) goto error_too_many_argc;

              //Теперь мы стоим на том arg куда нужно вписать аргумент.
              /*  (this_tree -> argv)[k] = malloc (CCIS (*(Word_List + i)) + 1);
                j = 0;
                while(*(Word_List + i)[j] != 0)
                  {
                  (this_tree -> argv)[k][j]  = *(Word_List + i)[j];
                  }
                (this_tree -> argv)[k][j] = 0;*/

              (this_tree -> argv)[k] = *(Word_List + i);

              //Таким образом мы создали ячейку древовидной структуры
              //и вписали название. Теперь состояние CONV (ТО ЖЕ ЧТО И БЫЛО)
              tmp_state = CONV;
            break;

            case PIPE:
              //Если мы столкнулись с PIPE, нужно перейти в состояние PIPE1;
              tmp_state = CONV1;
            break;

            case SMALLER:
              tmp_state = IN;
            break;

            case BIGGER:
              tmp_state = OUT;
            break;

            case DOUBLEBIGGER:
              tmp_state = OUT_APP;
            break;

            case AMPERSANT:
              tmp_state = BACKGRND;
            break;

            case COMMADOT:
              tmp_state = POINTCOMMA;
            break;

            default:
              goto error_syntax;
            break;
            }
          }
      break;

      case CONV1:
        //printf ("11112020_CONV1\n");

        //ЗДЕСЬ У НАС ПУСТОЕ ЭТО ОШИБКА - незавершенный конвейер
        if (*(Word_List + i) == NULL)
          {
          //printf ("NULLLIST %s\n", *(Word_List + i));
          goto error_syntax;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //Смотрим какая это строка
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //Вызываем функцию создания ячейки, сохранив предыдущую ссылку
              prev_tree = this_tree;
              this_tree = create_empty_node();
              prev_tree -> pipe = this_tree;

              //А именно скопировать всё что нужно в  argv.

              //Выделяем по длине строки(не включая 0) и 1 символ для \0

              /*(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
               j = 0;
              while(*(Word_List + i)[j] != 0)
                {
                (this_tree -> argv)[0][j]  = *(Word_List + i)[j];
                }
              (this_tree -> argv)[0][j] = 0;*/

              (this_tree -> argv)[0] = *(Word_List + i);

              //Таким образом мы создали ячейку древовидной структуры
              //и вписали название. Теперь состояние CONV

              tmp_state = CONV;
            break;

            default:
              goto error_syntax;
            break;
            }
          }
      break;


      case POINTCOMMA:
        //printf ("11112020_POINTCOMMA\n");
        if (*(Word_List + i) == NULL)
          {
          //Это может быть конец ввода а не синтаксическая ошибка
          //goto error_syntax;
          i--; //Чтобы на следующем ходе цикла просмотреть этот же элемент
          tmp_state = CONV;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //Смотрим какая это строка
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //Вызываем функцию создания ячейки, сохранив предыдущую ссылку
              /*prev_tree = this_tree;
              this_tree = create_empty_node();
              prev_tree -> next = this_tree;*/


              this_tree = create_empty_node();
              pointcomma_tree -> next = this_tree;
              prev_tree = pointcomma_tree;
              pointcomma_tree = this_tree;


              //А именно скопировать всё что нужно в  argv.

              //Выделяем по длине строки(не включая 0) и 1 символ для \0
              /*(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
              j = 0;
              while(*(Word_List + i)[j] != 0)
                {
                (this_tree -> argv)[0][j]  = *(Word_List + i)[j];
                }
              (this_tree -> argv)[0][j] = 0;*/

              //(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
              (this_tree -> argv)[0] = *(Word_List + i);

              //Таким образом мы создали ячейку древовидной структуры
              //и вписали название. Теперь состояние CONV


              tmp_state = CONV;
            break;

            default:
              goto error_syntax;
            break;
            }
          }
      break;

      case IN:
        //printf ("11112020_IN\n");
        if (*(Word_List + i) == NULL)
          {
          goto error_syntax;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //Смотрим какая это строка
          switch (this_class)
            {
            case FILE_OR_COMMAND:

              //Выделяем по длине строки(не включая 0) и 1 символ для \0
              /*(this_tree -> infile) = malloc (CCIS (*(Word_List + i)) + 1);
              j = 0;
              while(*(Word_List + i)[j] != 0)
                {
                (this_tree -> infile)[j]  = *(Word_List + i)[j];
                }
              (this_tree-> infile)[j] = 0;*/

              (this_tree -> infile) = *(Word_List + i);

              //Таким образом мы создали ячейку древовидной структуры
              //и вписали название. Теперь состояние CONV

              tmp_state = IN1;
            break;

            default:
              goto error_syntax;
            break;
            }
          }
      break;


      case IN1:
        //printf ("11112020_IN1\n");
        if (*(Word_List + i) == NULL)
          { //Это может быть конец ввода а не синтаксическая ошибка
          //goto error_syntax;
          i--; //Чтобы на следующем ходе цикла просмотреть этот же элемент
          tmp_state = CONV;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //Смотрим какая это строка
          switch (this_class)
            {
            case BIGGER:
              tmp_state = OUT;
            break;

            default:
              i--; //Чтобы на следующем ходе цикла просмотреть этот же элемент
              tmp_state = CONV;
            break;
            }
          }
      break;


      case OUT:
        //printf ("11112020_OUT\n");
        if (*(Word_List + i) == NULL)
          {
          goto error_syntax;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //Смотрим какая это строка
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //Записываем вывод
              (this_tree -> outfile) = *(Word_List + i);

              tmp_state = OUT1;
            break;

            default:
              goto error_syntax;
            break;
            }
          }
      break;

      case OUT1:
        //printf ("11112020_OUT1\n");
        if (*(Word_List + i) == NULL)
          {
          i--; //Чтобы на следующем ходе цикла просмотреть этот же элемент
          tmp_state = CONV;
          }
        else
          {
          tmp_state = CONV;
          i--; //Чтобы на следующем ходе цикла просмотреть этот же элемент находясь в CONV
          }
      break;



      case OUT_APP:
        //printf ("11112020_OUT_APP\n");
        if (*(Word_List + i) == NULL)
          {
          goto error_syntax;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //Смотрим какая это строка
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //Записываем вывод
              (this_tree -> outfile) = *(Word_List + i);
              this_tree -> append = 1; //Это и есть append.

              tmp_state = OUT1;
            break;

            default:
              goto error_syntax;
            break;
            }
          }
      break;


      case BACKGRND:
        //printf ("11112020_BACKGRND\n");
        if (*(Word_List + i) == NULL)
          {
          //Не забываем о background для comma;
          //и для всех её последователей по конвейеру
          //pointcomma_tree -> backgrnd = 1;
          set_tree_backgrnd_1 (pointcomma_tree);

          tmp_state = END;
          }
        else
          {
          goto error_syntax;
          }
      break;

      case END:
        ; //L-граф...
      break;

      default:
        printf ("%d", tmp_state); //Обработка ошибки, default быть не должно
      break;
      }

    //И КОНЕЧНО НЕ ЗАБЫВАЕМ О ТОМ ЧТО В ЦИКЛЕ МЫ ИДЕМ ПО СПИСКУ
    i++;

    //printf ("\n");
    //print_tree (tree_root, 5);
    //printf ("\n");

    //printf ("ITERCYCLE");
    }

  return tree_root;

  error_syntax:
  printf ("Syntax error");
  return 0;


  error_too_many_argc:
  printf ("Argv error");
  return 0;
  }

  //Поскольку дерево привязано к списку, удалением строк займется очиститель списка
void delete_tree (tree this_tree)
  {
  if (this_tree == NULL) return;
  else
    {
    /*for (int i = 0; (this_tree -> argv)[i] != NULL; i++)
      {
      free ( (this_tree -> argv)[i] );
      } */
    free (this_tree -> argv); //Удаляем массив ссылок


    /*   struct cmd_inf* psubcmd; // команды для запуска в дочернем shell
    struct cmd_inf* pipe; // следующая команда после "|"
    struct cmd_inf* next;*/

    delete_tree (this_tree -> psubcmd);
    delete_tree (this_tree -> pipe);
    delete_tree (this_tree -> next);

    /*if (this_tree -> infile != NULL)
      {
      free (this_tree -> infile); */
      this_tree -> infile = NULL;
      //}
    //if (this_tree -> outfile != NULL)
      //{
      //free (this_tree -> outfile);
      this_tree -> outfile = NULL;
      //}
    }

  free (this_tree);
  }

