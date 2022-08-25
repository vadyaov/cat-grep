#include "s21_cat.h"

int main(int argc, char **argv) {
    if (argc > 1) {
        flag flags;
        init(&flags);
        int i = 1;
        int errorFlag = 0;
        for (; i < argc; i++) {
            if (*argv[i] != '-') break;
            errorFlag = catchFlag(&flags, argv[i]);
        }
        if (flags.b) flags.n = 0;
        if (i == argc && !errorFlag) printf("s21_cat: please enter the file name");
        if (errorFlag) printf("s21_cat: unknown flag");
        FILE *file;
        for (; i < argc && !errorFlag; i++) {
            file = fopen(argv[i], "r");
            if (file) {
                useFlags(file, &flags);
                fclose(file);
            } else {
                printf("s21_cat: can't open file %s\n", argv[i]);
            }
        }
    } else {
        printf("s21_cat: not enough arguments");
    }
    return 0;
}

void useFlags(FILE *file, flag *flags) {
    int b = 1;
    int n = 1;
    int isStart = 1;
    char previous[3] = "\n\n";  // prev[0] - last, prev[1] - before last
    char c[5] = {'\0'};  // for long symbols
    while ((c[0] = fgetc(file)) != EOF) {
        if (flags->s) {
            if (flag_s(previous[0], previous[1], c[0])) continue;
        }
        if (flags->n) {
            isStart = flag_n(isStart, *c, &n);
        }
        if (flags->b) {
            flag_b(previous[0], *c, &b);
        }
        previous[1] = previous[0];
        previous[0] = *c;
        if (flags->e) {
            flag_e(*c);
        }
        if (flags->v) {
            flag_v(c);
        }
        if (flags->t) {
            if (flag_t(*c)) continue;
        }
        if (!*c) fputc(*c, stdout);
        else
            fputs(c, stdout);  // fputs can't copy null character
        set_zeros(c, 5);
    }
}

void set_zeros(char *arr, int length) {
    for (int i = 0; i < length; i++) {
        arr[i] = '\0';
    }
}

void flag_v(char *c) {
    unsigned char s = *c;
    if (s <= 8 || (s >= 11 && s <= 31) || (s >= 127 && s <= 159))
        strcpy(c, specialSymbols[s]);
}

int flag_t(char c) {
    int a = 0;
    if (c == '\t') {
        putc('^', stdout);
        putc('I', stdout);
        a = 1;
    }
    return a;
}

void flag_e(char c) {
    if (c == '\n') putc('$', stdout);
}

int flag_s(char last, char bef_last, char c) {
    int r = 0;
    if (last == '\n' && bef_last == '\n' && c =='\n') r = 1;
    return r;
}

int flag_n(int isStart, char c, int *n) {
    int res = 0;
    if (isStart) printf("%6d\t", (*n)++);
    if (c == '\n') res = 1;
    return res;
}

void flag_b(char last, char c, int *b) {
    if (last == '\n' && c != '\n')
        printf("%6d\t", (*b)++);
}

int catchFlag(flag *flags, char *argv) {
    int error = 0;
    if (!strcmp(argv, "--number-nonblank") || !strcmp(argv, "-b")) {
        flags->b = 1;
    } else if (!strcmp(argv, "--number") || !strcmp(argv, "-n")) {
        flags->n = 1;
    } else if (!strcmp(argv, "--squeeze-blank") || !strcmp(argv, "-s")) {
        flags->s = 1;
    } else if (!strcmp(argv, "-e")) {
        flags->e = 1;
        flags->v = 1;
    } else if (!strcmp(argv, "-E")) {
        flags->e = 1;
    } else if (!strcmp(argv, "-t")) {
        flags->t = 1;
        flags->v = 1;
    } else if (!strcmp(argv, "-T")) {
        flags->t = 1;
    } else if (!strcmp(argv, "-v")) {
        flags->v = 1;
    } else {
        error = 1;
    }
    return error;
}

void init(flag *data) {
    data->b = 0;
    data->e = 0;
    data->n = 0;
    data->s = 0;
    data->t = 0;
    data->v = 0;
}
