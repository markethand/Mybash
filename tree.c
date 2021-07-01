#include "tree.h"


tree create_empty_node (void)
  {
  // �������, �� �������� ������
  tree this_tree = malloc (1 * sizeof (node));
  // �� �������� ������ ��� ��������� ����������

  //������� ������ 10 ��������� ���������� �� char
  //                     (��. ��������)
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

    //=================���������� ������ �� ������ ������ ==============
  //���������: ����� ��������, ��� ������ � � ������������

  //�������� tree � node
  tree this_tree = NULL; // ������ ����� ���� � ������, ������ - ������
  tree tree_root = NULL; // �� ������ ���� ���������� ������� ������ ������
  tree prev_tree = NULL; // ���������� ����������� ���������
  tree pointcomma_tree = NULL; // ��������� �� �������� ������ (������ � ��������)

  //�������� ���������� ��� ������������� �����
  enum STR_CLASSIFIER this_class;



  tmp_state = BEGIN;
  i = 0; // ������ ����� ��������� �� ������ �� �����


  //���� ����� � ������ ������ �� �����������,
  //����� ��������� �� �� ������ �������� ���������.
  while (tmp_state != END)
    {
    //printf ("11112020_CYCLE\n");
    //i - �������, ������ � ������� ������ ��������
    switch (tmp_state)
      {
      case BEGIN:
        //printf ("11112020_BEGIN\n");
        //��������� ���������, i = 0;
        /*������� � ��� 2:
          1. Word_List ����
            ����� ��������� ���������� ������.
          2. Word_List ������
            ����� ������� ���� � �������������� ��������� �������
            �  ���� ��� ������� �������� ���������  �� CONV */
        if (*(Word_List + i) == NULL) tmp_state = END;
        else
          {
          //��� ������ �� �����, ������ ����� ����������� ���� ��� �������, ���� ��� �������
            //__________________       ���������       ______________________
            // ����� ��������, ��� �������� ��������, �� ������������� �������� classificator
            // ���� ��� ���� ��� ������� (������ ��� � �� ����) �� ��


          if (classificator (*(Word_List + i)) == FILE_OR_COMMAND)
            {
            //�������� ������� �������� ������.
            this_tree = create_empty_node();
            tree_root = this_tree;
            pointcomma_tree = this_tree;


            //� ������ ����������� �� ��� ����� �  argv.

            //�������� �� ����� ������(�� ������� 0) � 1 ������ ��� \0
            /*(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
            j = 0;
            while(*(Word_List + i)[j] != 0)
              {
              (this_tree -> argv)[0][j]  = *(Word_List + i)[j];
              }
            (this_tree -> argv)[0][j] = 0;*/


            (this_tree -> argv)[0] = *(Word_List + i);

            //����� ������� �� ������� ������ ����������� ���������
            //� ������� ��������. ������ ��������� CONV

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
        //��������� ���������� � ���������
                 // � ������ ���������� ������, ����������, � � �
        //������ ����� ���������, ������� ��� �� �������.
        //� ���� �������, ����������� ����

        if (*(Word_List + i) == NULL)
          {
          tmp_state = END;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //������� ����� ��� ������
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //printf ("FILE_OR_COMMAND:%s \n", *(Word_List + i));

              //��� ����� �� ��� � �� ������������ ��� ��� ��������
              //����� ��� ����� �������� ��������
              //��� ��� ��� �������?
              //��� ����� ������ � ������� argv �� NULL,

              //��������� ��������� �.�. ������� ��������� ����� ��������.
              for (k = 0; (k < 10)&&( (this_tree -> argv)[k] != NULL); k++);                                                 //------------------------------------------------����� 10
              if (k == 10) goto error_too_many_argc;

              //������ �� ����� �� ��� arg ���� ����� ������� ��������.
              /*  (this_tree -> argv)[k] = malloc (CCIS (*(Word_List + i)) + 1);
                j = 0;
                while(*(Word_List + i)[j] != 0)
                  {
                  (this_tree -> argv)[k][j]  = *(Word_List + i)[j];
                  }
                (this_tree -> argv)[k][j] = 0;*/

              (this_tree -> argv)[k] = *(Word_List + i);

              //����� ������� �� ������� ������ ����������� ���������
              //� ������� ��������. ������ ��������� CONV (�� �� ��� � ����)
              tmp_state = CONV;
            break;

            case PIPE:
              //���� �� ����������� � PIPE, ����� ������� � ��������� PIPE1;
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

        //����� � ��� ������ ��� ������ - ������������� ��������
        if (*(Word_List + i) == NULL)
          {
          //printf ("NULLLIST %s\n", *(Word_List + i));
          goto error_syntax;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //������� ����� ��� ������
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //�������� ������� �������� ������, �������� ���������� ������
              prev_tree = this_tree;
              this_tree = create_empty_node();
              prev_tree -> pipe = this_tree;

              //� ������ ����������� �� ��� ����� �  argv.

              //�������� �� ����� ������(�� ������� 0) � 1 ������ ��� \0

              /*(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
               j = 0;
              while(*(Word_List + i)[j] != 0)
                {
                (this_tree -> argv)[0][j]  = *(Word_List + i)[j];
                }
              (this_tree -> argv)[0][j] = 0;*/

              (this_tree -> argv)[0] = *(Word_List + i);

              //����� ������� �� ������� ������ ����������� ���������
              //� ������� ��������. ������ ��������� CONV

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
          //��� ����� ���� ����� ����� � �� �������������� ������
          //goto error_syntax;
          i--; //����� �� ��������� ���� ����� ����������� ���� �� �������
          tmp_state = CONV;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //������� ����� ��� ������
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //�������� ������� �������� ������, �������� ���������� ������
              /*prev_tree = this_tree;
              this_tree = create_empty_node();
              prev_tree -> next = this_tree;*/


              this_tree = create_empty_node();
              pointcomma_tree -> next = this_tree;
              prev_tree = pointcomma_tree;
              pointcomma_tree = this_tree;


              //� ������ ����������� �� ��� ����� �  argv.

              //�������� �� ����� ������(�� ������� 0) � 1 ������ ��� \0
              /*(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
              j = 0;
              while(*(Word_List + i)[j] != 0)
                {
                (this_tree -> argv)[0][j]  = *(Word_List + i)[j];
                }
              (this_tree -> argv)[0][j] = 0;*/

              //(this_tree -> argv)[0] = malloc (CCIS (*(Word_List + i)) + 1);
              (this_tree -> argv)[0] = *(Word_List + i);

              //����� ������� �� ������� ������ ����������� ���������
              //� ������� ��������. ������ ��������� CONV


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

          //������� ����� ��� ������
          switch (this_class)
            {
            case FILE_OR_COMMAND:

              //�������� �� ����� ������(�� ������� 0) � 1 ������ ��� \0
              /*(this_tree -> infile) = malloc (CCIS (*(Word_List + i)) + 1);
              j = 0;
              while(*(Word_List + i)[j] != 0)
                {
                (this_tree -> infile)[j]  = *(Word_List + i)[j];
                }
              (this_tree-> infile)[j] = 0;*/

              (this_tree -> infile) = *(Word_List + i);

              //����� ������� �� ������� ������ ����������� ���������
              //� ������� ��������. ������ ��������� CONV

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
          { //��� ����� ���� ����� ����� � �� �������������� ������
          //goto error_syntax;
          i--; //����� �� ��������� ���� ����� ����������� ���� �� �������
          tmp_state = CONV;
          }
        else
          {
          this_class = classificator (*(Word_List + i));

          //������� ����� ��� ������
          switch (this_class)
            {
            case BIGGER:
              tmp_state = OUT;
            break;

            default:
              i--; //����� �� ��������� ���� ����� ����������� ���� �� �������
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

          //������� ����� ��� ������
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //���������� �����
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
          i--; //����� �� ��������� ���� ����� ����������� ���� �� �������
          tmp_state = CONV;
          }
        else
          {
          tmp_state = CONV;
          i--; //����� �� ��������� ���� ����� ����������� ���� �� ������� �������� � CONV
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

          //������� ����� ��� ������
          switch (this_class)
            {
            case FILE_OR_COMMAND:
              //���������� �����
              (this_tree -> outfile) = *(Word_List + i);
              this_tree -> append = 1; //��� � ���� append.

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
          //�� �������� � background ��� comma;
          //� ��� ���� � �������������� �� ���������
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
        ; //L-����...
      break;

      default:
        printf ("%d", tmp_state); //��������� ������, default ���� �� ������
      break;
      }

    //� ������� �� �������� � ��� ��� � ����� �� ���� �� ������
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

  //��������� ������ ��������� � ������, ��������� ����� �������� ���������� ������
void delete_tree (tree this_tree)
  {
  if (this_tree == NULL) return;
  else
    {
    /*for (int i = 0; (this_tree -> argv)[i] != NULL; i++)
      {
      free ( (this_tree -> argv)[i] );
      } */
    free (this_tree -> argv); //������� ������ ������


    /*   struct cmd_inf* psubcmd; // ������� ��� ������� � �������� shell
    struct cmd_inf* pipe; // ��������� ������� ����� "|"
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

