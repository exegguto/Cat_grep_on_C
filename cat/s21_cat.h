#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

typedef struct Opt {
  int b, e, n, s, t, v;
  int list_files[100];
} opt;

void parser(opt *options, int argc, char *argv[]);
void reading(char **argv, opt *option);
void usage(char *name);

#endif  // SRC_CAT_S21_CAT_H_
