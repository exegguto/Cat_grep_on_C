#include "s21_cat.h"

#include <getopt.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  opt options = {0};
  parser(&options, argc, argv);
  reading(argv, &options);
  return (0);
}

/**
 * Обработка результатов getopt и заполнение параметров
 * @param options это структура с параметрами, именами файлов, и регуляркой
 * @param argc количество элементов в командной строке
 * @param argv это массив указателей на нуль-терминированные строки, содержащие
 * параметры командной строки, с которыми вызывалась программа.
 */
void parser(opt *options, int argc, char *argv[]) {
  int i = 0;
  while (optind != argc) {
    static struct option long_opt[] = {{"number-nonblank", 0, 0, 'b'},
                                       {"squeeze-blank", 0, 0, 's'},
                                       {"number", 0, 0, 'n'},
                                       {0, 0, 0, 0}};
    int optIdx;
    switch (getopt_long(argc, argv, "+benstvET", long_opt, &optIdx)) {
      case 'b':
        options->b = 1;
        options->n = 0;
        break;
      case 'e':
        options->e = 1;
        options->v = 1;
        break;
      case 'E':
        options->e = 1;
        options->v = 0;
        break;
      case 'n':
        if (options->b == 0) options->n = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 't':
        options->t = 1;
        options->v = 1;
        break;
      case 'T':
        options->t = 1;
        options->v = 0;
        break;
      case 'v':
        options->v = 1;
        break;
      default:
        if (optind < argc) {
          if (*argv[optind] == '-')
            usage(argv[optind]);
          else
            options->list_files[i++] = optind;
          optind++;
        }
        break;
    }
  }
}

/**
 * Обработка результатов getopt и заполнение параметров
 * @param options это структура с параметрами, именами файлов, и регуляркой
 * @param argv это массив указателей на нуль-терминированные строки, содержащие
 * параметры командной строки, с которыми вызывалась программа.
 */
void reading(char **argv, opt *option) {
  int str_count = 0, i = 0;
  while (option->list_files[i] != '\0') {
    FILE *f = fopen(argv[option->list_files[i++]], "r");
    if (f) {
      int str_num, begin_line = 1;
      while ((str_num = fgetc(f)) != EOF) {
        if (option->s && begin_line && str_num == '\n') {
          while ((str_num = fgetc(f)) != EOF && str_num == '\n') {
          }
          if (option->n) printf("%6d\t", ++str_count);
          if (option->e) printf("$");
          printf("\n");
        }
        if (begin_line && ((option->b && str_num != '\n') || option->n))
          printf("%6d\t", ++str_count), begin_line = 0;
        if (str_num == 10) {
          begin_line = 1;
          if (option->e) printf("$");
        } else {
          begin_line = 0;
          switch (str_num) {
            case 0 ... 8:
            case 10 ... 31:
              if (option->v) printf("^"), str_num += 64;
              break;
            case 9:
              if (option->t) printf("^"), str_num = 73;
              break;
            case 127:
              if (option->v) printf("^"), str_num -= 64;
              break;
            case 128 ... 255:
              if (option->v) printf("M-"), str_num -= 128;
              break;
          }
        }
        printf("%c", str_num);
      }
    } else {
      printf("cat: %s: No such file or directory\n",
             argv[option->list_files[i - 1]]);
    }
  }
}

/**
 * Выводит сообщение о верном использовании функции
 * @param name имя файла
 */
void usage(char *name) {
  printf(
      "usage: %s\n\t - h this message\n\t - c[config file]\n"
      "\t-- help this message\n\t-- config = config_file\n",
      name);
}
