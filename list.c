#include "list.h"

//ГЛОБАЛЬНЫЕ



//WORK WITH STR IS VERY USEFUL TO BE ORGANISED AS WITH GLOBAL
int tmp_STR_SIZE = STR_SIZE; // if fread < STR SIZE we have to know we have to stop
char str [STR_SIZE] = {};
int str_position = -1;
int read_symbols = STR_SIZE;
int END_FLAG = 0; // =1, if the file ended

char DHOME_STRING[] = "$HOME";
char DSHELL_STRING[] = "$SHELL";
char DUSER_STRING[] = "$USER";
char DEUID_STRING[] = "$EUID";

//состояние ENDFLAG

int GIVE_END_FLAG(void) {return END_FLAG;}


//ВСПОМОГАТЕЛЬЫЕ ФУНКЦИИ
//LIST REALLOCATION FUNCTION
//Reallocate This Word List
void RTWL (char *** Word_List_link, int * Word_list_size_link)
  {
  //DEBUG
  //int TMP_WLSL = *Word_list_size_link;
  //static int RTWL_COUNT = 0;
  //RTWL_COUNT++;
  //char * DAV = NULL; //Debug auxilary varuable


  static char **tmp_list = NULL;

  tmp_list = malloc ((*Word_list_size_link + WORD_QUANTITY) * sizeof (char*));
  for (int i = 0; i < *Word_list_size_link; i++)
    {
    //DAV = *(      (*Word_List_link) + i * sizeof (char*));
    //*(tmp_list + ((long) i) ) = DAV;
    *(tmp_list + i) = *(      (*Word_List_link) + i);
    }
  *Word_list_size_link += WORD_QUANTITY;


  /*for (int i = 0; *(*Word_List_link + i) != 0; i++)
    {
    free ( *(*Word_List_link + i ));
    }*/
  free (*Word_List_link);

  *Word_List_link = tmp_list;

  return;
  }


//WRITE from_buf_to_list
void WFBTL(char ** word_list_unit_link, char * word_buf, int word_buf_position)
  {
  *(word_list_unit_link) = malloc ((word_buf_position + 2) * sizeof (char));

  char * word_list_unit = *(word_list_unit_link);

  for (int j = 0; j <= word_buf_position; j++)
    {
    *(word_list_unit + j) = *(word_buf + j);
    }
  *(word_list_unit + (word_buf_position + 1)  ) = '\0';


  //printf ("%s\n", *(word_list_unit_link));
  return;
  }


char getsym(void) //only for reading symbols from str
  {
  char buf_sym;

  if (fread(&buf_sym, 1, 1, stdin) == 0)
    {
    END_FLAG = 1;
    return '\n';
    }
  else
    {
    return buf_sym;
    }
  }



//======================  СТРОКОВЫЕ    ФУНКЦИИ     ====================


//FIRST STRING BIGGER LEXICOGRAPHICALLY (B > A) (AB > A)
//1 if yes, 0 if no
//if they are equal returns 0
int FSBL (char * first_string, char * second_string)
  {
  int i = 0;

  while ((*(first_string + i) != '\0'))
    {
    //if (*(second_string + i * sizeof(char)) == '\0') return 0;
    if (*(first_string + i) > *(second_string + i)) return 1;
    if (*(first_string + i) < *(second_string + i)) return 0;

    i++;
    }

  return 0;
  }
    //TEST STRINGS FOR FSBL
    //char str1[] = "|";
    //char str2[] = "||";


//Count Chars In String (not including zero)
int CCIS (char * str)
  {
  int i = 0;

  while (*(str + i) != '\0') i++;

  return i;
  }

//Count Words In List
int CWIL (char ** List)
  {
  int i = 0;

  while (*(List + i) != NULL) i++;

  return i;
  }


//DEBUG FUNCTIONS
void debug_list_print(char ** Word_List, int Word_list_position)
  {
  printf ("_");
  for (int i = 0; i < Word_list_position; i++)
    {
    //printf ("0");
    printf ("%s\n", *(Word_List + i));
    //printf ("1");
    }
  printf ("_");
  return;
  }



// 1 если равны
// 0 если не равны
int cmp_str (char * first_string, char * second_string)
  {
  int i = 0;

  while (*(first_string + i) == *(second_string + i))
    {
    if (*(first_string + i) == '\0') return 1;
    i++;
    }

  return 0;
  }

//Сравнивает строки до i того символа
int cmp_str_part (char * first_string, char * second_string, int size)
  {
  int i = 0;

  while (i < size)
    {
    if (*(first_string + i) != *(second_string + i)) return 0;
    i++;
    }

  return 1;

  }


char ** list_create(void) // Возвращает адрес начала массива char*
  {
  int i, j, k;

  char *auxilary = NULL;
  char tmp_sym = 0;

  //If lack of space for word pointers, we ll realloc word_buf
  char **Word_List = NULL;
  Word_List = malloc (WORD_QUANTITY * sizeof (char*));
  int Word_list_size = WORD_QUANTITY; //for easy reallocation
  int Word_list_position = 0;
    char **tmp_list = NULL;

  //If lack of space for word, we ll realloc word_buf
  char *word_buf = NULL;
  word_buf = malloc (WORD_MAX_SIZE * sizeof (char*));
  int word_buf_size = WORD_MAX_SIZE; // for easy reallocation
  int word_buf_position = 0;
    // New buffer for reallocation of word_buf
    char *tmp_buf = NULL;


  char SS[2] = {};
  int SS_Q = 0;





  //NOVEMBER UPGRAGES:
  int SINGLE_QUOTE_STATE = 0; //WE ARE NOT BETWEEN ' QUOTES
  int DOUBLE_QUOTE_STATE = 0; //WE ARE NOT BETWEEN "" QUOTES
  int SLASH_STATE = 0; //ХРАНИМ СЛЭШ
  int HASH_STATE = 0; //ХРАНИМ РЕШЕТКУ
  char *str_insert = NULL;
  int DOLLAR_STATE = 0; //ХРАНИМ, БЫЛ ЛИ ЗАСЛЕШИРОВАН ПОСЛЕДНИЕ ДОЛЛАР
  int DOLLAR_LENGTH = 0; //ХРАНИМ ДЛИНУ ЗАДОЛЛИРОВАННОГО СЛОВА


  // MAIN CYCLE
  // Here we read words to word_buf
  for (word_buf_position = 0; !END_FLAG; word_buf_position++)    {
    //debug_list_print (Word_List, Word_list_position);

    if (word_buf_position == word_buf_size)
      {
      //we can not write and we need to reallocate word_buf
      tmp_buf = malloc ((word_buf_size + WORD_MAX_SIZE) * sizeof (char*));
      for (int i = 0; i < word_buf_size; i++) *(tmp_buf + i) = *(word_buf + i);
      word_buf_size += WORD_MAX_SIZE;
      free (word_buf);
      word_buf = tmp_buf;
      tmp_buf = NULL;
      }


    //*(word_buf + word_buf_position) = getsym();
    //tmp_sym = *(word_buf + word_buf_position);
    tmp_sym = getsym ();

    //...................ОБРАБОТКА РЕШЕТКИ.................
    // до конца строки проматываем дальше опускаем HASH_STATE
    if (HASH_STATE && (tmp_sym != '\n'))
      { //word_buf_position не должен увеличиваться
      //Мы хотим ИГНОРИРОВАТЬ эти символы
      word_buf_position = -1;
      continue;
      }
    else if (tmp_sym == '\n')
      {
      HASH_STATE = 0;
      }

    //отладочный вывод
    //printf ("DEBUG_%s\n", word_buf);

    //-------------------------ОБРАБОТКА СЛЭША------------
    // Заметка: слеш может быть внутри кавычки
    // слэш можно ПРИМЕНИТЬ только в свободном от кавычек месте
    // таким образом нужно добавить слэш в главный СВИТЧ
    // и когда встречается кавычка или слэш, проверять,
    // не был ли сохранён слэш.
    // и во всех остальных случаях, просто ?? ИГНОРИРОВАТЬ СЛЭШ


    //--------------------ОБРАБОТКА КАВЫЧЕК-081120----------------
    //  для обработка кавычек будем писать в buf как писали


    //ЗАМЕЧАНИЕ: СЛЕШ ОБРАБАТЫВАЕМ ТОЛЬКО ЧЕРЕЗ СВИТЧ
    //ТАК ЖЕ КАК И ВОЗМОЖНУЮ КАВЫЧКУ ()
    if (SINGLE_QUOTE_STATE && (tmp_sym != '\\') && (!SLASH_STATE)) //мы внутри одинарной кавычки
      {
      //если мы уже внутри кавычки
      //то если наш tmp_sym - кавычка, то заканчиваем ввод и добавляем слово.
      //если нет, то просто пишем в buf очередной символ

      if (tmp_sym == '\'')
        { //кавычные внутренности кончились
        //Запишем слово в память всё что мы имели до этого
        if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
        WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
        Word_list_position++;

        word_buf_position = -1; //На следующей интерации цикла будет 0.
        SINGLE_QUOTE_STATE = 0;
        }
      else
        {
        //Увеличивать word_buf_position не надо, это происходит в цикле
        *(word_buf + word_buf_position) = tmp_sym;
        }
      }
    else if (DOUBLE_QUOTE_STATE && (tmp_sym != '\\') && (!SLASH_STATE)) //мы внутри двойной кавычки
      {
      //если мы уже внутри двойной кавычки
      //то если наш tmp_sym - кавычка, то заканчиваем ввод и добавляем слово.
      //если нет, то просто пишем в buf очередной символ

      if (tmp_sym == '"')
        { //кавычные внутренности закончились
        //Запишем слово в память всё что мы имели до этого
        if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
        WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
        Word_list_position++;

        word_buf_position = -1; //На следующей интерации цикла будет 0.
        DOUBLE_QUOTE_STATE = 0;
        }
      else
        {
        //Увеличивать word_buf_position не надо, это происходит в цикле
        *(word_buf + word_buf_position) = tmp_sym;
        }

      }
    else
      {
      //Если мы не находились внутри кавычки
      //То мы смотрим, ' " или другое
      //это можно сделать через тот case который уже был написан


        switch (tmp_sym)
          {
          case '|':
          case '&':
          case '>':
            SLASH_STATE = 0;

            if (SS_Q && (SS[0] == tmp_sym))
              { //WE CAN WRITE TWO SYMBOLS (\0 will be added by WFBTL)
              if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
              SS[1] = tmp_sym;
              WFBTL ((Word_List + Word_list_position), SS, 1);
              Word_list_position++;
              SS_Q = 0;

              word_buf_position = -1;
              break;
              }
            else if (SS_Q)
              { //WE HAVE TO WRITE ONE SS
              if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
              WFBTL ((Word_List + Word_list_position), SS, 0);
              Word_list_position++;

              //AND STORE THAT WAS READED
              SS[0] = tmp_sym;  //SS_Q = 1
              word_buf_position = -1;
              break;
              }
            else
              {
              if (word_buf_position > 0)
                { //IF THERE IS ANYTHING TO BE WRITTEN
                if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
                Word_list_position++;
                }

              SS[0] = tmp_sym;
              SS_Q = 1;

              word_buf_position = -1;
              }
            break;



          case ';':
          case '<':
          case '(':
          case ')':
            SLASH_STATE = 0;

            if (SS_Q)
              { //IF SPECIAL SYMBOL WAS SAVED BEFORE
              if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
              WFBTL ((Word_List + Word_list_position), SS, 0);
              Word_list_position++;
              SS_Q = 0;
              }
            else
              { //IF ANOTHER WORD WAS SAVED BEFORE
              if (word_buf_position > 0)
                {
                if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
                Word_list_position++;
                }
              }

            //WRITING THE SYMBOL ITSELF (SS EMPTY, SS_Q = 0)
            if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
            SS[0] = tmp_sym;
            WFBTL ((Word_List + Word_list_position), SS, 0);
            Word_list_position++;

            word_buf_position = -1;
            break;


          case '\0':
          case  ' ':
            SLASH_STATE = 0;

            if (SS_Q)
              {
              if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
              WFBTL ((Word_List + Word_list_position), SS, 0);
              Word_list_position++;
              SS_Q = 0;
              }
            else
              {
              if (word_buf_position > 0)
                {
                if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
                Word_list_position++;
                }
              }

            word_buf_position = -1;
            break;


          case '\n':
            //Если у нас был слэш, нужно игнорировать перенос строки
            if (SLASH_STATE)
              {
              SLASH_STATE = 0;
              word_buf_position--;
              }
            else
              {
              SLASH_STATE = 0;

                //--------------------ОБРАБОТКА КАВЫЧКИ-------------------
                //будем считать, что все кавычки сбалансированы
                //а если человеку нужно поставить одну кавычку
                //то он воспользуется слешированием

                if (SS_Q)
                  {
                  if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                  WFBTL ((Word_List + Word_list_position), SS, 0);
                  Word_list_position++;
                  SS_Q = 0;
                  }
                else
                  {
                  if (word_buf_position > 0)
                    {
                    if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                    WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
                    Word_list_position++;
                    }
                  }

                //PLACE FOR LIST SORT ETC
                *(Word_List + Word_list_position) = NULL;

                //And now lets check what we have
                /*printf ("\n _________________________ \n");
                for (int i = 0; i < Word_list_position; i++)
                  {
                  printf ("%s\n", *(Word_List + i));
                  }*/

                //printf ("%d", FSBL (str1, str2) );

                //SORTING THE MASSIV
                /* for (int i = 0; i < Word_list_position; i++)
                  {
                  for (int j = i + 1; j < Word_list_position; j++)
                    {
                    if (FSBL (*(Word_List + i), *(Word_List + j)))
                      {
                      auxilary = *(Word_List + i);
                      *(Word_List + i) = *(Word_List + j);
                      *(Word_List + j) = auxilary;
                      }
                    }
                  } */

                /*printf ("--------------------------------------------------");
                printf ("%d\n", CWIL(Word_List));
                for (int i = 0; i < Word_list_position; i++)
                  {
                  printf ("%s_%d\n", *(Word_List + i), CCIS(*(Word_List + i)));
                  }*/

                /* for (int i = 0; i < Word_list_position; i++)
                  {
                  free ( *(Word_List + i ));
                  }

                Word_list_position = 0;
                word_buf_position = -1; */

              goto function_exit; //------------------------------------------------------------------------------------ВРЕМЕННАЯ МЕРА
              }
          break;


          //-----------------ОБРАБОТКА СЛЭША---------------------
          case '\\':
            if (SLASH_STATE)
              { //Впишем слешик
              *(word_buf + word_buf_position) = tmp_sym;
              SLASH_STATE = 0;
              }
            else
              {
              //Выходит, мы записали слеш в буфер. -- позицию
              word_buf_position--;
              SLASH_STATE = 1;
              }
          break;


          //------------------ОБРАБОТКА КАВЫЧЕК------------------
          //И вот тут мы рассматриваем соответственно случаи что мы не были
          //Внутри кавычки а теперь мы в какой то кавычке
          case '\'':
            //Мы должны записать спецсимвол если таковой был

            if (SLASH_STATE) //Если действие кавычки отменено
              {
              *(word_buf + word_buf_position) = tmp_sym;
              SLASH_STATE = 0;
              }
            else
              {
              if (SS_Q)
                {
                if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                WFBTL ((Word_List + Word_list_position), SS, 0);
                Word_list_position++;
                SS_Q = 0;
                }
              else
                {
                if (word_buf_position > 0)
                  {
                  if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                  WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
                  Word_list_position++;
                  }
                }

              SINGLE_QUOTE_STATE = 1;
              word_buf_position = -1;
              }

          break;

          case '"':
            //Мы должны записать спецсимвол если таковой был
            if (SLASH_STATE) //Если действие кавычки отменено
              {
              *(word_buf + word_buf_position) = tmp_sym;
              SLASH_STATE = 0;
              }
            else
              {
              if (SS_Q)
                {
                if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                WFBTL ((Word_List + Word_list_position), SS, 0);
                Word_list_position++;
                SS_Q = 0;
                }
              else
                {
                if (word_buf_position > 0)
                  {
                  if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                  WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
                  Word_list_position++;
                  }
                }

              DOUBLE_QUOTE_STATE = 1;
              word_buf_position = -1;
              }
          break;


          //------------------ОБРАБОТКА СИМВОЛА РЕШЕТКИ..............

          case '#':

            if (SLASH_STATE) //Если действие решетки отменено
              {
              *(word_buf + word_buf_position) = tmp_sym;
              SLASH_STATE = 0;
              }
            else
              {
              if (SS_Q)
                {
                if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                WFBTL ((Word_List + Word_list_position), SS, 0);
                Word_list_position++;
                SS_Q = 0;
                }
              else
                {
                if (word_buf_position > 0)
                  {
                  if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
                  WFBTL ((Word_List + Word_list_position), word_buf, word_buf_position - 1);
                  Word_list_position++;
                  }
                }

              HASH_STATE = 1;
              word_buf_position = -1;
              }

          break;


          default:
            //printf ("DEBUG%c_%d_%d_%s\n", tmp_sym, DOLLAR_STATE, SLASH_STATE, word_buf);

            //---------------------ОБРАБОТКА СЛЕШИРОВАНИЯ ДОЛЛАРА-----------------
            if ((tmp_sym == '$')&& (SLASH_STATE)) DOLLAR_STATE = 0; //доллар неактивен
            else if (tmp_sym == '$')
              {
              DOLLAR_STATE = 1; // доллар активен
              DOLLAR_LENGTH = -1;
              }

            if (DOLLAR_STATE) DOLLAR_LENGTH++;

            SLASH_STATE = 0;

            //printf ("DEBUG_2%c_%d_%d_%s_%d\n", tmp_sym, DOLLAR_STATE, SLASH_STATE, word_buf, DOLLAR_LENGTH);

            //--------------------ОБРАБОТКА СЛУЧАЯ С ПОДСТАНОВКОЙ $HOME ТАКОГО ТИПА ШТУК--------------
            // Нужно записать в буфер слова подстановку $HOME $SHELL $USER $EUID
            // Очевидно тут есть один нюанс - буфер не должен переполниться в процессе добавления
            // Такого рода штук. Поэтому в цикле будем делать проверку на то не кончился ли буфер
            /* if (word_buf_position == word_buf_size)
                  {
                  //we can not write and we need to reallocate word_buf
                  tmp_buf = malloc ((word_buf_size + WORD_MAX_SIZE) * sizeof (char*));
                  for (int i = 0; i < word_buf_size; i++) *(tmp_buf + i) = *(word_buf + i);
                  word_buf_size += WORD_MAX_SIZE;
                  free (word_buf);
                  word_buf = tmp_buf;
                  }                           */



            if (SS_Q)
              {
              if (Word_list_position == Word_list_size) RTWL (&Word_List, &Word_list_size);
              WFBTL ((Word_List + Word_list_position), SS, 0);
              Word_list_position++;
              SS_Q = 0;
              }

            *(word_buf + word_buf_position) = tmp_sym;

            //и теперь сверяем не имеем ли мы одну из наших переменных при активном долларе
            if (DOLLAR_STATE && (DOLLAR_LENGTH == 5)) //Можем иметь $SHELL
              {
              //Сверяем последние 6 символов с $SHELL
              if ( cmp_str_part(word_buf + word_buf_position - 5, DSHELL_STRING, 6))
                {
                //Стираем часть буфера, меняя word_buf_position
                word_buf_position -= 5; //Стоим на $
                //И пишем вместо $SHELL То что вызовем с помощью getenv

                str_insert = getenv("SHELL");
                //printf ("DEVUG09112020_%s\n",getenv("SHELL"));
                //printf ("DEVUG09112020_%s\n",word_buf);

                M_STRINGINSERT(6);
                DOLLAR_STATE = 0;
                }
              else
                {
                DOLLAR_STATE = 0;
                DOLLAR_LENGTH = 0;
                }
              }
            else if (DOLLAR_STATE && (DOLLAR_LENGTH == 4))
              {
              //---------------------------H O M E ---------------------------------

              if ( cmp_str_part(word_buf + word_buf_position - 4, DHOME_STRING, 5))
                {
                word_buf_position -= 4;
                str_insert = getenv("HOME");

                M_STRINGINSERT(5);
                DOLLAR_STATE = 0;
                }


              //-------------------------U S E R -------------------------------------

              else if ( cmp_str_part(word_buf + word_buf_position - 4, DUSER_STRING, 5))
                {
                word_buf_position -= 4;
                str_insert = getenv("USER");

                M_STRINGINSERT(5);
                DOLLAR_STATE = 0;
                }

              //--------------------------E U I D ------------------------------

              else if ( cmp_str_part(word_buf + word_buf_position - 4, DEUID_STRING, 5))
                {
                word_buf_position -= 4;
                str_insert = getenv("EUID");
                DOLLAR_STATE = 0;

                M_STRINGINSERT(5);
                }
              else
                { //Не обнуляем пока что ничего по той причине что это может быть SHELL
                //DOLLAR_STATE = 0;
                //DOLLAR_LENGTH = 0;
                ;
                }
              }
            else if (DOLLAR_LENGTH >= 5) //Если мы дошли до сюда значит это точно не переменная
              {
              DOLLAR_STATE = 0;
              DOLLAR_LENGTH = 0;
              }
            break;
          }
        }
    }

  function_exit:

  //Нужно уничтожить выделенную функцией память.
  free (word_buf);

  //И дальше, возвращаем список.
  return Word_List;
  }





void list_delete(char *** word_list_link) // Получает ссылку на ячейку, удаляет список
                                         // И далее записывает NULL в ячейку
  {
  char ** Word_List = *word_list_link;
  for (int i = 0; *(Word_List + i) != 0; i++)
    {
    free ( *(Word_List + i ));
    }

  free (Word_List);
  *word_list_link = NULL;
  }




void list_print (char ** Word_List)
  {
  printf ("--------------------------------------------------");
  printf ("%d\n", CWIL(Word_List));
  for (int i = 0; *(Word_List + i) != NULL; i++)
    {
    printf ("%s_%d\n", *(Word_List + i), CCIS(*(Word_List + i)));
    }
  }
