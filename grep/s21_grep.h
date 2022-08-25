#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define SIZE 8912

typedef struct flag {
    int e;
    int i;
    int v;
    int c;
    int l;
    int n;
    int h;
    int s;
    int f;
    int o;
}flag;

int catchFlag(char *argv, flag *flags);
void takeFlags(flag *flags, char **argv, int *errorFlag);
char *takeTempales(char *tempales, char **argv, int *was_ef);
char *fillTemp(char *tempales, char *argv);
void FileFunctions(char *filename, flag *flags, regex_t *regex, int one);
void parsefiles(char **argv, flag *flags, regex_t *regex, int filenum, int ef);
int howmanyfiles(char **argv, int ef);
void regCreate(regex_t *regex, flag *flags, char *tempales);
int putEnd(char *tempales);
int nofnoe(char **argv, int i);
void init(flag *flags);
void ifSega(const char *argv);
int takeflag(char fl, flag *flags);
int doubleflag(char *argv, flag *flags);
char *filework(char *arg, char *tempales);

#endif  // SRC_GREP_S21_GREP_H_
