#include "list.h"


enum type_of_next
  {
  NXT, AND, OR   // ���� ������ �������� ������ � ������ ������
  };

struct cmd_inf
  {
  //���� ������� ��� ������� ����� ������� �� 10 ����������
  char ** argv; // ������ �� ����� ������� � ����������
  char *infile; // ��������������� ���� ������������ �����
  char *outfile; // ��������������� ���� ������������ ������
  int append;
  int backgrnd; // =1, ���� ������� �������� ���������� � ������� ������
  struct cmd_inf* psubcmd; // ������� ��� ������� � �������� shell
  struct cmd_inf* pipe; // ��������� ������� ����� "|"
  struct cmd_inf* next; // ��������� ����� ";" (��� ����� "&")
  enum type_of_next type;// ����� �� ��������� �������� ����� ; ��� && ��� ||
  };

typedef struct cmd_inf *tree;
typedef struct cmd_inf node;



tree create_empty_node (void);
void set_tree_backgrnd_1 (tree this_tree);

void make_shift(int n);
void print_argv(char **p, int shift);
void print_tree(tree t, int shift);


//------------------��� ������ ��� L-����� ��������������� �����----------

enum LGRAPH_STATE
  {
  BEGIN,    //��������� ���������
  CONV,     //��������� ���������� � ���������
            // � ������ ���������� ������, ����������, � � �
  CONV1,    //��������������� ��� ���������


  IN,       //��������� �������� ����� ����� �����
  OUT,      //��������� �������� ����� ����� ������
  OUT_APP,  //��������� �������� ����� ����� ������ ��� ��������

  IN1,
  OUT1,

  BACKGRND, //��������� �����

  POINTCOMMA, //��������� ����� � �������. ����� ����� next
  END       //
  };



//-------------------�������-������������� ���������� ���������------------

enum STR_CLASSIFIER
  {
  PIPE,
  BIGGER,
  DOUBLEBIGGER,
  SMALLER,
  AMPERSANT,
  COMMADOT,
  FILE_OR_COMMAND,

  ERROR // ���� �������� ������������ �������
        // ��� ������������ ���������� ������
        // �������� abc(cba - ������
        // B ������ ������ ����������� ||, && � ������
  };

int CCIST (char * str);

enum STR_CLASSIFIER classificator (char *this_string);


tree create_tree (char **Word_List);
void delete_tree (tree this_tree);





