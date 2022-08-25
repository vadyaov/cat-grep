CC	=	gcc
FLAGS	=	-Wall	-Werror	-Wextra	-std=c11	-pedantic	-fanalyzer	#-g	-fsanitize=address

all:	rebuild	test

s21_cat:
	$(CC)	$(FLAGS)	s21_cat.c	-o	s21_cat

test:
	./t_cat.sh

clean:
	rm	-rf	s21_cat

rebuild:	clean	s21_cat