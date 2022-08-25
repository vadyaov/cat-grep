CC	=	gcc
FLAGS	=	-Wall	-Werror	-Wextra	-std=c11	-pedantic	-fanalyzer	#-g	-fsanitize=address

all:	rebuild	test

s21_grep:
	$(CC)	$(FLAGS)	s21_grep.c	-o	s21_grep

test:	rebuild
	./t_grep.sh
	./script.sh

clean:
	rm	-rf	s21_grep

rebuild:	clean	s21_grep