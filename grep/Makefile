CC=gcc
DIRECTORY=pwd
CFLAGS=-std=c11 -Wall -Werror -Wextra
TC_FLAGS=-lpcre
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

all: clean s21_grep

s21_grep: s21_grep.a
	$(CC) s21_grep.a -o s21_grep $(TC_FLAGS)

s21_grep.a: s21_grep.o
	ar rcs s21_grep.a s21_grep.o 
	ranlib s21_grep.a

s21_grep.o: s21_grep.c
	$(CC) $(FLAG1) $(CFLAGS) $(FLAG2) s21_grep.c -o s21_grep.o 

clean:
	rm -rf  *.a *.o *.out *.info *.gcno *.gcda report *.cfg val/ s21_grep

check:
	chmod +x t_grep.sh
	chmod +x t_grepMac.sh
	cppcheck --enable=all --suppress=missingIncludeSystem *.h *.c
	cp ../../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../../materials/linters/cpplint.py --extension=c *.c *.h
	$(LEAK_CHECK) ./s21_grep for s21_grep.c s21_grep.h Makefile -v -c -o
