#include "list.h"


enum type_of_next
  {
  NXT, AND, OR   // виды связей соседних команд в списке команд
  };

struct cmd_inf
  {
  //Буду считать что команда может принять до 10 аргументов
  char ** argv; // список из имени команды и аргументов
  char *infile; // переназначенный файл стандартного ввода
  char *outfile; // переназначенный файл стандартного вывода
  int append;
  int backgrnd; // =1, если команда подлежит выполнению в фоновом режиме
  struct cmd_inf* psubcmd; // команды для запуска в дочернем shell
  struct cmd_inf* pipe; // следующая команда после "|"
  struct cmd_inf* next; // следующая после ";" (или после "&")
  enum type_of_next type;// связь со следующей командой через ; или && или ||
  };

typedef struct cmd_inf *tree;
typedef struct cmd_inf node;



tree create_empty_node (void);
void set_tree_backgrnd_1 (tree this_tree);

void make_shift(int n);
void print_argv(char **p, int shift);
void print_tree(tree t, int shift);


//------------------ТИП ДАННЫХ ДЛЯ L-ГРАФА СИНТАКСИЧЕСКОГО ЭТАПА----------

enum LGRAPH_STATE
  {
  BEGIN,    //Начальное состояние
  CONV,     //Состояние нахождения в конвейере
            // а именно считывание файлов, аргументов, и т д
  CONV1,    //Вспомогательное для конвейера


  IN,       //Состояние ожидания имени файла ввода
  OUT,      //Состояние ожидания имени файла вывода
  OUT_APP,  //Состояние ожидания имени файла вывода для дозаписи

  IN1,
  OUT1,

  BACKGRND, //Состояние когда

  POINTCOMMA, //Состояние точки с запятой. Связь через next
  END       //
  };



//-------------------ФУНКЦИЯ-КЛАССИФИКАТОР ОЧЕРЕДНОГО АРГУМЕНТА------------

enum STR_CLASSIFIER
  {
  PIPE,
  BIGGER,
  DOUBLEBIGGER,
  SMALLER,
  AMPERSANT,
  COMMADOT,
  FILE_OR_COMMAND,

  ERROR // Если содержит недопустимые символы
        // или недопустимые комбинации лексем
        // например abc(cba - ошибка
        // B данной версии недопустимы ||, && и другие
  };

int CCIST (char * str);

enum STR_CLASSIFIER classificator (char *this_string);


tree create_tree (char **Word_List);
void delete_tree (tree this_tree);





