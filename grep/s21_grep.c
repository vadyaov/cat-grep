#include "s21_grep.h"

int main(int argc, char **argv) {
    if (argc > 2) {
        int errorFlag = 0, ef = 0, filenum = 0;
        char *tempales = NULL;
        flag flags;
        regex_t regex;
        init(&flags);
        takeFlags(&flags, argv, &errorFlag);
        if (errorFlag) {
            printf("grep: unknown flag\n");
            exit(EXIT_FAILURE);
        }
        tempales = takeTempales(tempales, argv, &ef);
        regCreate(&regex, &flags, tempales);
        filenum = howmanyfiles(argv, ef);
        parsefiles(argv, &flags, &regex, filenum, ef);
    } else {
        printf("grep: not enough arguments\n");
    }
    return 0;
}

void parsefiles(char **argv, flag *flags, regex_t *regex, int filenum, int ef) {
    int i = 1;
    int skipfirst = 0;
    int flag = 0;
    for (; argv[i]; i++) {
        if (!ef && *argv[i] != '-' && !skipfirst) {
            skipfirst = 1;
            continue;
        }
        if (i > 2 && (!strcmp(argv[i - 2], "-e") || !strcmp(argv[i - 2], "-f"))
            && (!strcmp(argv[i - 1], "-e") || !strcmp(argv[i - 1], "-f"))) flag = 1;
        if (*argv[i] != '-' && (nofnoe(argv, i) || flag)) FileFunctions(argv[i], flags, regex, filenum);
        flag = 0;
    }
    regfree(regex);
}

int putEnd(char *tempales) {
    return (tempales && tempales[strlen(tempales) - 1] != '\0' && tempales[strlen(tempales) - 1] != '\n');
}

int nofnoe(char **argv, int i) {
    int res = 0;
    if (strcmp(argv[i - 1], "-e") && strcmp(argv[i - 1], "-f")) res = 1;
    if (*argv[i - 1] == '-' && strlen(argv[i - 1]) == 3 && (argv[i - 1][2] == 'e' || argv[i - 1][2] == 'f'))
        res = 0;
    return res;
}

void regCreate(regex_t *regex, flag *flags, char *tempales) {
    int val;
    if (flags->i) val = regcomp(regex, tempales, REG_ICASE | REG_EXTENDED);
    else
        val = regcomp(regex, tempales, REG_EXTENDED);
    if (val) {
        printf("Could not compile regex\n");
        regfree(regex);
        exit(1);
    }
    free(tempales);
}

int howmanyfiles(char **argv, int ef) {
    int i = 1;
    int count = 0;
    int skipfirst = 0;
    for (; argv[i]; i++) {
        if (!ef && *argv[i] != '-' && !skipfirst) {
            skipfirst = 1;
            continue;
        }
        if (i > 2 && *argv[i] != '-' && !strcmp(argv[i - 2], "-e") && !strcmp(argv[i - 1], "-f")) count++;
        if (*argv[i] != '-' && nofnoe(argv, i)) count++;
    }
    if (count > 1) count = 0;
    return count;
}

void FileFunctions(char *filename, flag *flags, regex_t *regex, int one) {
    FILE *file;
    regmatch_t match[1] = {0};
    file = fopen(filename, "r");
    if (file) {
        char str[SIZE] = {'\0'};
        int count = 0;
        int thisFile = 0;
        int lineCount = 0;
        while (fgets(str, SIZE, file) != NULL) {
            lineCount++;
            if (!strchr(str, '\n')) strcat(str, "\n");
            if (!flags->v) {
                if (!regexec(regex, str, 1, match, 0) && !flags->l && !flags->c) {
                    if (!one && !flags->h) printf("%s:", filename);
                    if (flags->n) printf("%d:", lineCount);
                    if (!flags->o) {
                        printf("%s", str);
                    } else {
                        char *pstr = str;
                        int error = 0;
                        while (!error) {
                            if (match[0].rm_eo == match[0].rm_so) break;
                            printf("%.*s\n", (int)(match[0].rm_eo - match[0].rm_so),
                            pstr + match[0].rm_so);
                            pstr += match[0].rm_eo;
                            error = regexec(regex, pstr, 1, match, REG_NOTBOL);
                        }
                    }
                }
                if (flags->c && !flags->l) {
                    if (!regexec(regex, str, 0, NULL, 0))
                        count++;
                }
                if (flags->l && !flags->c) {
                    if (!regexec(regex, str, 0, NULL, 0))
                        thisFile = 1;
                }
                if (flags->l && flags->c) {
                    if (!regexec(regex, str, 0, NULL, 0)) {
                        count++;
                        thisFile = 1;
                        break;
                    }
                }
            } else {
                if (regexec(regex, str, 0, NULL, 0) && !flags->l && !flags->c) {
                    if (!one && !flags->h) printf("%s:", filename);
                    if (flags->n) printf("%d:", lineCount);
                    printf("%s", str);
                }
                if (flags->c && !flags->l) {
                    if (regexec(regex, str, 0, NULL, 0))
                        count++;
                }
                if (flags->l && !flags->c) {
                    if (regexec(regex, str, 0, NULL, 0))
                        thisFile = 1;
                }
                if (flags->l && flags->c) {
                    if (regexec(regex, str, 0, NULL, 0)) {
                        count++;
                        thisFile = 1;
                        break;
                    }
                }
            }
            memset(str, '\0', SIZE);
        }
        if (flags->c && !flags->l) {
            if (one) {
                printf("%d\n", count);
            } else {
                if (!flags->h) printf("%s:", filename);
                printf("%d\n", count);
            }
        }
        if (flags->l && !flags->c && thisFile) {
            printf("%s\n", filename);
        }
        if (flags->l && flags->c) {
            if (one) {
                printf("%d\n", count);
                if (thisFile) printf("%s\n", filename);
            } else {
                if (!flags->h) printf("%s:", filename);
                printf("%d\n", count);
                if (thisFile) printf("%s\n", filename);
            }
        }
        fclose(file);
    } else {
        if (!flags->s) printf("grep: %s: No such file or directory\n", filename);
    }
}

void takeFlags(flag *flags, char **argv, int *errorFlag) {
    int i = 1;
    for (; argv[i]; i++) {
        if (*argv[i] == '-') *errorFlag = catchFlag(argv[i], flags);
        if (*errorFlag) break;
        if (flags->e || flags->f) {
            if (flags->e) flags->e = 0;
            if (flags->f) flags->f = 0;
            i++;
        }
    }
}

char *takeTempales(char *tempales, char **argv, int *was_ef) {
    int i = 1;
    for (int j = 1; argv[j]; j++) {
        if (!strcmp(argv[j], "-e") || !strcmp(argv[j], "-f")) *was_ef = 1;
        if (*argv[i] == '-' && strlen(argv[i]) == 3 && (argv[i][1] == 'f' || argv[i][2] == 'f' ||
            argv[i][1] == 'e' || argv[i][2] == 'e')) *was_ef = 1;
    }
    int ef = *was_ef;
    for (; argv[i]; i++) {
        if (!ef && *argv[i] != '-') {
            tempales = fillTemp(tempales, argv[i]);
            ef = 1;
        }
        if (*argv[i] == '-' && strlen(argv[i]) == 3 && (argv[i][1] == 'e' || argv[i][2] == 'e')) {
            if (argv[i][1] == 'e') {
                tempales = fillTemp(tempales, argv[i] + 2);
            } else {
                ifSega(argv[i + 1]);
                i++;
                tempales = fillTemp(tempales, argv[i]);
                }
        }
        if (!strcmp(argv[i], "-e") && strcmp(argv[i - 1], "-f")) {
            ifSega(argv[i + 1]);
            i++;
            tempales = fillTemp(tempales, argv[i]);
        }
        if (*argv[i] == '-' && strlen(argv[i]) == 3 && (argv[i][1] == 'f' || argv[i][2] == 'f')) {
            if (argv[i][1] == 'f') {
                tempales = filework(argv[i] + 2, tempales);
            } else {
                ifSega(argv[i + 1]);
                i++;
                tempales = filework(argv[i], tempales);
            }
        }
        if (!strcmp(argv[i], "-f") && strcmp(argv[i - 1], "-e")) {
            ifSega(argv[i + 1]);
            i++;
            tempales = filework(argv[i], tempales);
        }
    }
    if (putEnd(tempales)) tempales[strlen(tempales) - 1] = '\0';
    return tempales;
}

char *filework(char *arg, char *tempales) {
    FILE *fp;
    fp = fopen(arg, "r");
    if (fp) {
        char str[SIZE] = {'\0'};
        while (fgets(str, SIZE, fp) != NULL) {
            if (strlen(str) > 1) {
                char *n = NULL;
                if ((n = strchr(str, '\n')) != NULL) *n = '\0';
            }
            tempales = fillTemp(tempales, str);
            memset(str, '\0', SIZE);
        }
        fclose(fp);
    } else {
        printf("grep: %s: No such file or directory\n", arg);
        exit(EXIT_FAILURE);
    }
    return tempales;
}

void ifSega(const char *argv) {
    if (!argv) {
        printf("grep: option requires an argument -- e\n");
        exit(EXIT_FAILURE);
    }
}

char* fillTemp(char *tempales, char *argv) {
    int size = strlen(argv) + 2, var = 0;
    if (size == 2) {
        var = 1;
        size += 1;
    }
    if (!tempales) {
        tempales = malloc(size);
        if (tempales) {
            if (var) strcat(tempales, "\n");
            else
                strcpy(tempales, argv);
            strcat(tempales, "|");
        }
    } else {
        int size1 = strlen(tempales);
        tempales = realloc(tempales, size + size1);
        if (tempales) {
            if (var) strcat(tempales, "\n");
            else
                strcpy(tempales + size1, argv);
            strcat(tempales, "|");
        }
    }
    return tempales;
}

int doubleflag(char *argv, flag *flags) {
    int error = 0;
    if (strlen(argv) == 3) {
        int i = 1;
        error = takeflag(argv[i], flags);
        i++;
        if (!error) error = takeflag(argv[i], flags);
    }
    return error;
}

int takeflag(char fl, flag *flags) {
    int err = 0;
    if (fl == 'i') flags->i = 1;
    else if (fl == 'v') flags->v = 1;
    else if (fl == 'c') flags->c = 1;
    else if (fl == 'l') flags->l = 1;
    else if (fl == 'n') flags->n = 1;
    else if (fl == 'h') flags->h = 1;
    else if (fl == 's') flags->s = 1;
    else if (fl == 'o') flags->o = 1;
    else if (fl == 'e') flags->e = 1;
    else if (fl == 'f') flags->f = 1;
    else
        err = 1;
    return err;
}

int catchFlag(char *argv, flag *flags) {
    int error = 0;
    if (strlen(argv) == 3) {
        error = doubleflag(argv, flags);  // without e && f
    } else {
        if (!strcmp(argv, "-e")) flags->e = 1;
        else if (!strcmp(argv, "-i")) flags->i = 1;
        else if (!strcmp(argv, "-v")) flags->v = 1;
        else if (!strcmp(argv, "-c")) flags->c = 1;
        else if (!strcmp(argv, "-l")) flags->l = 1;
        else if (!strcmp(argv, "-n")) flags->n = 1;
        else if (!strcmp(argv, "-h")) flags->h = 1;
        else if (!strcmp(argv, "-s")) flags->s = 1;
        else if (!strcmp(argv, "-f")) flags->f = 1;
        else if (!strcmp(argv, "-o")) flags->o = 1;
        else
            error = 1;
    }
    return error;
}

void init(flag *flags) {
    flags->c = 0;
    flags->e = 0;
    flags->i = 0;
    flags->l = 0;
    flags->n = 0;
    flags->v = 0;
    flags->h = 0;
    flags->s = 0;
    flags->f = 0;
    flags->o = 0;
}
