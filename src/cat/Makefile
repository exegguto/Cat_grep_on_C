CC=gcc
DIRECTORY=pwd
CFLAGS=-std=c11 -Wall -Werror -Wextra
TC_FLAGS=
LEAK_CHECK=
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CC += -D LINUX -g
	LEAK_CHECK += valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind_log.txt
endif
ifeq ($(UNAME_S),Darwin)
    CC += -D OSX
	LEAK_CHECK +=CK_FORK=no leaks -atExit --
endif

#-wall лучше снять перед вертером

FLAG1=-I src
FLAG2=-c

default: all

all: clean s21_cat

s21_cat: s21_cat.a
	$(CC) s21_cat.a -o s21_cat $(TC_FLAGS)

s21_cat.a: s21_cat.o
	ar rcs s21_cat.a s21_cat.o
	ranlib s21_cat.a

s21_cat.o: s21_cat.c
	$(CC) $(FLAG1) $(CFLAGS) $(FLAG2) s21_cat.c -o s21_cat.o

check:
	chmod +x t_cat.sh
	chmod +x t_catMac.sh
	cppcheck --enable=all --suppress=missingIncludeSystem *.h *.c
	cp ../../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../../materials/linters/cpplint.py --extension=c *.c *.h
	$(LEAK_CHECK) ./s21_cat -vtes bytes.txt

clean:
	rm -rf  *.a *.o *.out *.info *.gcno *.gcda report *.cfg s21_cat val/
