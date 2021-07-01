#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define DEBUGMODE 1
#define FILEMODE 0
#define CYCLEMODE 0
#define DEBUGLGRAPH 1


//ABOUT BUFFER STRING
#define STR_SIZE 20 // THE STR BUF SIZE

//ABOUT WORDS THAT WE READ
#define WORD_QUANTITY  150
#define WORD_MAX_SIZE 20

#define M_STRINGINSERT(M_SIZE)  \
      /* DEBUG  OUTPUT */\
      printf ("DEBUG_%s\n", str_insert);\
      if (str_insert != NULL)\
      { \
      \
      for (int tmp = 0; *(str_insert + tmp) != 0; word_buf_position++, tmp++)\
        {\
        if (word_buf_position == word_buf_size)\
          { \
          tmp_buf = malloc ((word_buf_size + WORD_MAX_SIZE)*sizeof (char*)); \
          for (int j = 0; j < word_buf_size; j++) *(tmp_buf + j) = *(word_buf + j); \
          word_buf_size += WORD_MAX_SIZE;\
          free (word_buf);\
          word_buf = tmp_buf;\
          }\
        *(word_buf + word_buf_position) = *(str_insert + tmp);\
        }\
      word_buf_position--;\
      }\
    else\
      {\
      word_buf_position -= M_SIZE; \
      }



int GIVE_END_FLAG(void);

void RTWL (char *** Word_List_link, int * Word_list_size_link);
void WFBTL(char ** word_list_unit_link, char * word_buf, int word_buf_position);
char getsym(void);
int FSBL (char * first_string, char * second_string);
int CCIS (char * str);
int CWIL (char ** List);
void debug_list_print(char ** Word_List, int Word_list_position);
int cmp_str (char * first_string, char * second_string);
int cmp_str_part (char * first_string, char * second_string, int size);



/* ИНТЕРФЕЙСНАЯ ЧАСТЬ
Какие функции нам понадобятся? */
/* Функция создания списка,
   функция удаления списка
   а отладочные функции я оставлю в WLM*/

char ** list_create(void); // Возвращает адрес начала массива char*
void    list_delete(char *** word_list_link); // Получает ссылку на ячейку, удаляет список
                                         // И далее записывает NULL в ячейку

void    list_print (char ** word_list);
