
#include "s21_grep.h"

#include <errno.h>
#include <getopt.h>
#include <pcre.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  opt options = {0};
  options.pattern = calloc(10000, sizeof(char));
  get_opt(&options, argc, argv);
  parser(&options, argv);
  if (options.pattern != 0) {
    reading(&options, argv);
    free(options.pattern);
  }
  return (0);
}

/**
 * Обработка результатов getopt и заполнение параметров
 * @param options это структура с параметрами, именами файлов, и регуляркой
 * @param argc количество элементов в командной строке
 * @param argv это массив указателей на нуль-терминированные строки, содержащие
 * параметры командной строки, с которыми вызывалась программа.
 */
void get_opt(opt *options, int argc, char **argv) {
  while (optind != argc) {
    char temp = getopt(argc, argv, "+eivclnhsfo");
    switch (temp) {
      case 'e':
        options->e = 1;
        options->patt[++options->patt[0]] = optind++;
        break;
      case 'i':
        options->i = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'c':
#ifndef OSX
        if (options->l == 0)
#endif
          options->c = 1, options->n = 0;
#ifdef OSX
        options->o = 0;
#endif
        break;
      case 'l':
        options->l = 1;
        options->n = 0;
        options->o = 0;
#ifndef OSX
        options->c = 0;
        options->h = 0;
#endif
        break;
      case 'n':
        if (options->l == 0 && options->c == 0) options->n = 1;
        break;
      case 'h':
#ifndef OSX
        if (options->l == 0)
#endif
          options->h = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 'f':
        options->f = 1;
        options->f_file[++options->f_file[0]] = optind++;
        break;
      case 'o':
#ifndef OSX
        if (options->l == 0) options->o = 1;
#else
        if (options->l == 0 && options->c == 0) options->o = 1;
#endif
        break;
      default:
        if (optind < argc) {
          if (*argv[optind] == '-')
            usage(argv[optind++]);
          else
            options->list_files[++options->list_files[0]] = optind++;
        }
        break;
    }
  }
}

/**
 * разбивает строку с параметрами, заполняя структуру и массивы с регуляркой и
 * файлами
 * @param options это структура с параметрами, именами файлов, и регуляркой
 * @param argv это массив указателей на нуль-терминированные строки,
 * содержащие параметры командной строки, с которыми вызывалась программа.
 */
void parser(opt *options, char **argv) {
  if (!(options->e) && !(options->f)) {
    options->e = 1;
    strcpy(options->pattern, argv[options->list_files[1]]);
    for (int j = 1; j < options->list_files[0]; j++)
      options->list_files[j] = options->list_files[j + 1];
    options->list_files[options->list_files[0]--] = 0;
  } else {
    int err = 1;
    unsigned int val_max = 9998;
    char *str_all = calloc(val_max, sizeof(char));
    for (int num = 1; num <= options->f_file[0] && err; num++) {
      FILE *fp = fopen(argv[options->f_file[num]], "r");
      if (fp) {
        int str_num = 0;
        while (str_num != EOF) {
          char *str = one_str(&str_num, fp);
          if (str_num != EOF || *str != '\0') copy_str(str, str_all, val_max);
          // printf("patt: %s\n", str);
          free(str);
          // printf("1patt: %s\n", str);
        }
        fclose(fp);
      } else {
        err = 0;
        if (!options->s)
          fprintf(stderr, "grep: %s: %s\n", argv[options->f_file[num]],
                  strerror(errno));
      }
    }
    if (options->patt[0] > 0) {
      for (int j = 1; j <= options->patt[0]; j++)
        copy_str(argv[options->patt[j]], str_all, val_max);
    }
    if (str_all[0] != '\0') strcat(options->pattern, "(");
    strcat(options->pattern, str_all);
    if (str_all[0] != '\0') strcat(options->pattern, ")");
    free(str_all);
    if (!err) free(options->pattern), options->pattern = 0;
  }
  // printf("pattern: %s\n", options->pattern);
}

/**
 * считывает строку из файла
 * @param str_num символ считанный из файла
 * @param f файл
 * @return возвращает сформированную строку
 */
char *one_str(int *str_num, FILE *f) {
  char *str = calloc(10000, sizeof(char));
  unsigned int val = 0, val_max = 9998;
  while ((*str_num = fgetc(f)) != '\n' && *str_num != EOF) {
    if (val == val_max) val_strlen(&val_max, " ", &str);
    str[val++] = *str_num;
  }
  str[val] = '\0';
  return str;
}

/**
 * добавляет к строке паттернов
 * @param val_max максимальная длинна строки
 * @param str строка паттернов, которую добавляем
 * @param str_all итоговая строка с паттернами
 * @param val_max выделенное количество памяти под строку паттернов
 */
void copy_str(char *str, char *str_all, unsigned int val_max) {
  if ((strlen(str_all) + strlen(str) + 2) > val_max)
    val_strlen(&val_max, str, &str_all);
  char *temp = calloc(strlen(str) + 3, sizeof(char));
  strcpy(temp, str);
  if (*temp == 0) strcpy(temp, "$");
  if (str_all[0] != '\0') strcat(temp, "|");
  memmove(str_all + strlen(temp) , str_all, strlen(str_all) + 1);
  memcpy(str_all, temp, strlen(temp));
  free(temp);
}

/**
 * увеличивает размер выделенной памяти
 * @param val_max максимальная длинна строки
 * @param str строка из-за которой выходит за границу
 * @param str_all строка, память которой нужно увеличить на str
 */
void val_strlen(unsigned *val_max, char *str, char **str_all) {
  *val_max += 99 + strlen(str);
  char *str_temp = realloc(*str_all, sizeof(char) * (*val_max + 1));
  if (str_temp) *str_all = str_temp;
}

/**
 * разбивает строку с параметрами, заполняя структуру и массивы с регуляркой и
 * файлами
 * @param options это структура с параметрами, именами файлов, и регуляркой
 * @param argv это массив указателей на нуль-терминированные строки,
 * содержащие параметры командной строки, с которыми вызывалась программа.
 */
void reading(opt *options, char **argv) {
  for (int i = 1; i <= options->list_files[0]; i++) {
    // имя файла
    char *argv_val = argv[options->list_files[i]];
    FILE *f = fopen(argv_val, "r");
    if (f) {
      int ovector[3000] = {0};
      int str_num = 0, repeat = 0, all_str = (options->n) ? 0 : -1;
      while (str_num != EOF) {
        read_str(argv_val, options, ovector, &str_num, f, &repeat, &all_str);
      }
      if (options->c) {
#ifdef OSX
        if (options->l && repeat > 0) repeat = 1;
#endif
        print_file(argv_val, options, ovector, NULL, repeat);
      }
#ifdef OSX
      if (options->l && repeat > 0)
        print_file(argv_val, options, ovector, NULL, -1);
#endif
      fclose(f);
    } else {
      if (!options->s)
        fprintf(stderr, "grep: %s: %s\n", argv_val, strerror(errno));
    }
  }
  argv[options->list_files[0]][strlen(argv[options->list_files[0]]) - 1] =
      '\0';  // костыль мак
}

/**
 * Функция осуществляет основные проверки со строкой прочитанной из файла
 * @param argv_val имя файла, с которым работаем
 * @param options это структура с параметрами, именами файлов, и регуляркой
 * @param ovector это массив указателей на диапазоны найденных подстрок
 * @param str_num код окончания файла и проверки
 * @param f файл
 * @param repeat счетчик повторений
 * @param all_str номер строки из файла, если -1, то не выводим
 */
void read_str(char *argv_val, opt options[], int *ovector, int *str_num,
              FILE *f, int *repeat, int *all_str) {
  char *str = one_str(str_num, f);
  int count = 1, pr = 0;
  count = grep_file(options, str, ovector);
  if (options->v && count <= 0 && !(*str_num == EOF && *str == 0)) {
#ifdef OSX
    if (!options->c) pr = 1, ovector[0] = 0, ovector[1] = strlen(str);
#else
    if (!options->c && !options->o) pr = 1;
#endif
    (*repeat)++;
  }
  if (count > 0 && !options->v && !(*str_num == EOF && *str == 0)) {
    if (!options->c) pr = 1;
#ifdef OSX
    if (options->o && strchr(options->pattern, '$'))
      pr = 1, ovector[0] = 0, ovector[1] = strlen(str);
#else
    if (options->o && ovector[0] == ovector[1]) pr = 0;
#endif
    (*repeat)++;
  }
  if (options->l && (pr || options->o)) {
#ifndef OSX
    pr = 1;
#else
    pr = 0;
#endif
    free(str);
    str = NULL;
    *str_num = EOF;
  }
  if (options->n) (*all_str)++;
  if (pr) print_file(argv_val, options, ovector, str, *all_str);
  if (str) free(str);
}

/**
 * поиск регулярного выражения в строке
 * @param options это структура с параметрами, именами файлов, и регуляркой
 * @param str строка для поиска регулярки
 * @param ovector это массив указателей на диапазоны найденных подстрок
 * @return количество совпадений
 */
int grep_file(opt *options, char *str, int *ovector) {
  // компилирование регулярного выражения во внутреннее представление
  pcre *re;
  int opt = 0, erroffset, ovec[3000], val = 0;
  const char *error;
  if (options->i) opt = PCRE_CASELESS;
  re = pcre_compile(options->pattern, opt, &error, &erroffset, NULL);
  if (re) {
    int count = pcre_exec(re, NULL, str, strlen(str), 0, 0, ovec, 3000);
    while (count > 0 && val < 3000) {
      ovector[val++] = ovec[0];
      ovector[val++] = ovec[1];
      if (ovec[0] == ovec[1] && ovec[1] <= (int)strlen(str)) ovec[1]++;
      count = pcre_exec(re, NULL, str, strlen(str), ovec[1], 0, ovec, 3000);
    }
  }
  pcre_free((void *)re);
  return val / 2;
}

/**
 * Выводит имя файла
 * @param name имя файла
 * @param n вывод номера строки
 */
void print_file(char *name, opt *options, int *ovector, char *str, int n) {
  // если файлов несколько и нет тега h (убрать названия файлов)
  int fpr = (options->list_files[0] > 1 && !options->h) ? 1 : 0;
  int s = 0, j = 0, prs = 1, color = isatty(1);
#ifdef OSX
    if (color) color = 0;  // Убирает цвет на маке
  if (options->h) fpr = 0;
#endif
  do {
    if (color) {
      if (fpr || (options->l && n == -1)) printf("\033[35m%s", name);
      if (fpr && !options->l) printf("\033[32m:");
    } else {
      if (fpr || (options->l && n == -1)) printf("%s", name);
#ifdef OSX
      if (fpr && (n != -1 || str != NULL)) printf(":");
#else
      if (fpr && !options->l) printf(":");
#endif
    }
    if (n != -1) {
      if (color) printf("\033[32m");
      if (options->c && color) printf("\033[0m");
      printf("%d", n);
      if (!options->c) printf(":");
    }
    if (color) printf("\033[0m");
    if (str != NULL) {
#ifdef OSX
      if (!strchr(options->pattern, '$')) {
        for (int i = 0, a = 0; str[i] != '\0'; i++) {
          if (i == ovector[a]) j = a, a = +2;
        }
      }
#endif
      for (int ex = 1; str[s] != '\0' && ex; s++) {
        if (color && !options->v && s == ovector[j]) printf("\033[31m");
        if (!((s < ovector[j] || s > ovector[j + 1]) && options->o))
          printf("%c", str[s]);
        if (!options->v && s == (ovector[j + 1] - 1)) {
          if (color) printf("\033[0m");
          j += 2;
          if (options->o && ovector[j] != ovector[j + 1]) ex = 0;
        }
      }
    }
    if (color) printf("\033[0m");
    printf("\n");
    if (!options->o || str == 0 || str[s] == '\0' ||
        (options->o && ovector[j] == 0))
      prs = 0;
  } while (prs);
  ovector[0] = 0;  // костыль мак
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
