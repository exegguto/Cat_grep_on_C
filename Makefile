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

all: clean s21_grep s21_cat 

s21_grep: s21_grep.a
	$(CC) grep/s21_grep.a -o grep/s21_grep $(TC_FLAGS)

s21_grep.a: s21_grep.o
	ar rcs grep/s21_grep.a grep/s21_grep.o 
	ranlib grep/s21_grep.a

s21_grep.o: grep/s21_grep.c
	$(CC) $(FLAG1) $(CFLAGS) $(FLAG2) grep/s21_grep.c -o grep/s21_grep.o 

s21_cat: s21_cat.a
	$(CC) cat/s21_cat.a -o cat/s21_cat

s21_cat.a: s21_cat.o
	ar rcs cat/s21_cat.a cat/s21_cat.o
	ranlib cat/s21_cat.a

s21_cat.o: cat/s21_cat.c
	$(CC) $(FLAG1) $(CFLAGS) $(FLAG2) cat/s21_cat.c -o cat/s21_cat.o

clean:
	rm -rf  *.a *.o *.out *.info *.gcno *.gcda report *.cfg 
	rm -rf grep/s21_grep grep/*.a grep/*.o grep/*.out grep/*.info grep/*.gcno grep/*.gcda grep/report grep/*.cfg grep/valgrind_log.txt
	rm -rf cat/s21_cat cat/*.a cat/*.o cat/*.out cat/*.info cat/*.gcno cat/*.gcda cat/report cat/*.cfg cat/valgrind_log.txt

check:
	cppcheck --enable=all --suppress=missingIncludeSystem grep/*.h grep/*.c
	cp ../materials/linters/CPPLINT.cfg grep/CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extension=c grep/*.c grep/*.h
	cppcheck --enable=all --suppress=missingIncludeSystem cat/*.h cat/*.c
	cp ../materials/linters/CPPLINT.cfg cat/CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extension=c cat/*.c cat/*.h
	chmod +x grep/t_grep.sh
	chmod +x grep/t_grepMac.sh
