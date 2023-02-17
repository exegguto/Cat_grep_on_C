#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <stdio.h>

typedef struct Opt {
  // Шаблон +
  int e;
  // Игнорирует различия регистра. +
  int i;
  // Инвертирует смысл поиска соответствий. +
  int v;
  // Выводит только количество совпадающих строк.+
  int c;
  // Выводит только совпадающие файлы. +
  int l;
  // Предваряет каждую строку вывода номером строки из файла ввода. +
  int n;
  // Выводит совпадающие строки, не предваряя их именами файлов. +
  int h;
  // Подавляет сообщения об ошибках о несуществующих или нечитаемых файлах. +
  int s;
  // Получает регулярные выражения из файла. +
  int f;
  // Печатает только совпадающие (непустые) части совпавшей строки.
  int o;
  // массив с порядковыми номерами файлов в командной строке
  int list_files[100];

  // массив порядковых номеров регулярных выражений в командной строке
  int patt[100];

  // строка со всеми регулярными выражениями
  char *pattern;
  // файл с регулярным выражением
  int f_file[100];
} opt;

#define FLAG "eivclnEIVCLN"

void get_opt(opt *options, int argc, char **argv);
char *one_str(int *str_num, FILE *f);
void parser(opt *options, char **argv);
void copy_str(char *str, char *str_all, unsigned int val_max);
void reading(opt *options, char **argv);
void read_str(char *argv_val, opt *options, int *ovector, int *str_num, FILE *f,
              int *repeat, int *all_str);
int grep_file(opt *options, char *str, int *ovector);
void print_file(char *name, opt *options, int *ovector, char *str, int n);
void usage(char *name);
void val_strlen(unsigned *val_max, char *str, char **str_all);

#endif  // SRC_GREP_S21_GREP_H_
