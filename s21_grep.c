#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define MAX_LEN 1024  // Предельная длина
#define CFL(i) (i == 0 ? REG_EXTENDED : REG_ICASE)

typedef struct {
    int i, v, c, l, n, h, s, o;
    int nf;  // количество файлов
    int sc;  // подсчет совпадающих строк
    int sn;  // подсчет строк в файле
} set;

int read_file(set *p, regex_t pr, char *filename);
void compare(set *p, regex_t pr, char *str, const char *filename);
void addition(int e, regex_t pr, char *str);
int my_getopt(int *num_arg, char **argv, set *p, char *template);
int process_ef(char c, int *num_arg, char *template, char **argv, int *i, int j);
char *get_substr(int s, int e, const char *str, char *substr);
int read_regex(char *template, char *part);
int modi_temp(char *template, char *part);
char *get_substr(int s, int e, const char *str, char *substr);

int ini_score(int *num_arg);
void get_put(void);
void err_out(int errnum);
void short_out(set p, const char *filename);
void output(set p, const char *str, const char *filename);

int main(int argc, char **argv) {
    if (argc > 1) {
        int num_arg[MAX_LEN] = {(argc - 1)}, err = 0;
        ini_score(num_arg);
        char template[MAX_LEN] = {0};
        set p = {0};
        err = my_getopt(num_arg, argv, &p, template);
        for (int i = 1; i <= num_arg[0]; i++) {
            if (num_arg[i] == 1) {
                if (strlen(template) == 0) {
                    err = modi_temp(template, argv[i]);
                    num_arg[i] = 0;
                } else {
                    p.nf++;
                }
            }
        }
        if (err == 0) {
            if (p.nf != 0) {
                regex_t pr;
                if (regcomp(&pr, template, CFL(p.i)) != 0)
                    err = 4;
                for (int i = 1; i <= num_arg[0]; i++) {
                    if (err != 0)
                        break;
                    if (num_arg[i] != 0)
                        err = read_file(&p, pr, argv[i]);
                }
                regfree(&pr);
            } else {
                get_put();
            }
        }
        if (err != 0)
            err_out(err);
    } else {
        err_out(5);
    }
    return 0;
}

// Функция построчно читает файл и отправляет строки для обработки
int read_file(set *p, regex_t pr, char *filename) {
    int err = 0;
    FILE *fptr = NULL;
    if ((fptr = fopen(filename, "r")) != NULL) {
        p->sn = p->sc = 0;
        while ((!feof(fptr)) && (err == 0)) {
            char str[MAX_LEN * 10 + 1] = {0};
            fgets(str, MAX_LEN * 10, fptr);
            if ((!feof(fptr)) || ((feof(fptr)) && (strlen(str) != 0))) {
                p->sn += 1;
                if (str[strlen(str) - 1] == '\n')
                    str[strlen(str) - 1] = '\0';
                compare(p, pr, str, filename);
            }
        }
        if ((p->c == 1) || (p->l == 1))
            short_out(*p, filename);
        fclose(fptr);
    } else {
        if (p->s != 1)
            err_out(2);
    }
    fptr = NULL;
    return err;
}

// Функция производит поиск template в str
void compare(set *p, regex_t pr, char *str, const char *filename) {
    regmatch_t pm;
    int flag = 0;
    if (regexec(&pr, str, 1, &pm, 0) == 0)
        flag = 1;
    if (p->v == 1) {
        flag = (flag == 1 ? 0 : 1);
        p->o = 0;
    }
    if (flag == 1) {
        if ((p->c != 1) && (p->l != 1)) {
            if (p->o != 1) {
                output(*p, str, filename);
            } else {
                char substr[MAX_LEN * 10 + 1] = {0};
                output(*p, get_substr(pm.rm_so, pm.rm_eo, str, substr), filename);
                addition(pm.rm_eo, pr, str);
            }
        } else {
            p->sc += 1;
        }
    }
}

// Функция производит поиск и вывод последующих вхождений подстроки
void addition(int e, regex_t pr, char *str) {
    while (1) {
        int len = strlen(str), i = e;
        if (len == e)
            break;
        for (; i <= len; i++)
            str[i - e] = str[i];
        regmatch_t pm;
        if (regexec(&pr, str, 1, &pm, 0) == 0) {
            char substr[MAX_LEN * 10] = {0};
//            if ((get_substr(pm.rm_so, pm.rm_eo, str, substr)) != NULL)
                printf("%s\n", get_substr(pm.rm_so, pm.rm_eo, str, substr));
        } else {
            break;
        }
        e = pm.rm_eo;
    }
}

// Функция получает флаги и их аргументы из командной строки и возвращает 0 при отсутствии ошибок
int my_getopt(int *num_arg, char **argv, set *p, char *template) {
    int err = 0;
    for (int i = 0; i <= num_arg[0]; i++) {
        if (err != 0)
            break;
        char *tmp = argv[i];
        if (*(tmp) == '-') {
            num_arg[i] = 0;
            char c;
            int j = 0;
            while ((c = tmp[++j]) != '\0') {
                p->i = (c == 'i' ? 1 : p->i);
                p->v = (c == 'v' ? 1 : p->v);
                p->c = (c == 'c' ? 1 : p->c);
                p->l = (c == 'l' ? 1 : p->l);
                p->n = (c == 'n' ? 1 : p->n);
                p->h = (c == 'h' ? 1 : p->h);
                p->s = (c == 's' ? 1 : p->s);
                p->o = (c == 'o' ? 1 : p->o);
                if ((c == 'e') || (c == 'f')) {
                    err = process_ef(c, num_arg, template, argv, &i, ++j);
                    break;
                }
            }
            p->l = (p->c == 1 ? 0 : p->l);
        }
    }
    return err;
}

// Функция обрабатывает флаги -e и -f
int process_ef(char c, int *num_arg, char *template, char **argv, int *i, int j) {
    int err = 0;
    char *tmp = argv[*i];
    char part[MAX_LEN] = {0};
    if (tmp[j] == 0) {
        if (*i < num_arg[0])
            *i += 1;
        else
            err = 3;
    }
    if (err == 0) {
        if (tmp[j] != 0) {
            get_substr(j, strlen(tmp), tmp, part);
        } else {
            if (strlen(argv[*i]) < MAX_LEN) {
                strcpy(part, argv[*i]);
                num_arg[*i] = 0;
            } else {
                err = 7;
            }
        }
    }
    if (err == 0) {
        if (c == 'e') {
            err = modi_temp(template, part);
        } else {
            err = read_regex(template, part);
        }
    }
    tmp = NULL;
    return err;
}

// Функция получет regex из файла
int read_regex(char *template, char *part) {
    int err = 0;
    FILE *fptr = NULL;
    if ((fptr = fopen(part, "r")) != NULL) {
        while ((!feof(fptr)) && (err == 0)) {
            char ar[MAX_LEN] = {0}, ch = 0;
            int i = 0;
            while (((ch = getc(fptr)) != '\n') && (ch != EOF)) {
                if (i < (MAX_LEN - 1))
                    ar[i++] = ch;
                else
                    err = 7;
            }
            ar[i] = '\0';
            if (err == 0) {
                if (ar[0] != '\0') {
                    err = modi_temp(template, ar);
                } else {
                    if (!feof(fptr))
                        err = modi_temp(template, "$");
                }
            }
        }
        fclose(fptr);
        fptr = NULL;
    } else {
        err = 2;
    }
    return err;
}

// Функция использует массив template
int modi_temp(char *template, char *part) {
    int err = 0, len_p = strlen(part), len_t = strlen(template);
    if ((len_p + len_t + 1) < MAX_LEN) {
        if (len_t == 0) {
            strcpy(template, part);
        } else {
            strcat(template, "|");
            strcat(template, part);
        }
    } else {
        err = 1;
    }
    return err;
}

// Функция записывает в substr подстроку
char *get_substr(int s, int e, const char *str, char *substr) {
    int len = e - s;
    for (int i = s; i < e; i++)
        substr[i - s] = str[i];
    substr[len] = '\0';
    return substr;
}

// Функция инициализации массива учета использования аргументов из командной строки
int ini_score(int *num_arg) {
    int err = 0;
    if (num_arg[0] < MAX_LEN) {
        for (int i = 1; i <= num_arg[0]; i++)
            num_arg[i] = 1;
    } else {
        err_out(6);
        num_arg[0] = 0;
    }
    return err;
}

// Функция принимает символы
void get_put(void) {
    char c;
    while ((c = getchar()) != EOF) continue;
}

// Функция вывода сообщений об ошибках
void err_out(int errnum) {
    char err_list[8][64] = {
        "s21_grep: undefined error: 0",
        "s21_grep: cannot allocate memory",
        "s21_grep: no such file or directory",
        "s21_grep: option requires an argument",
        "s21_grep: regex error",
        "usage: s21_grep [-eivclnhso] [pattern] [-f file] [file ...]",
        "s21_grep: there are a lot of arguments",
        "s21_grep: long regular expression"
    };
    perror(err_list[errnum]);
}

//  Вывод результатов при -c и -h
void short_out(set p, const char *filename) {
    if (p.l == 1) {
        if (p.sc != 0)
            printf("%s\n", filename);
    } else {
        if (p.nf > 1)
            printf("%s:", filename);
        printf("%d\n", p.sc);
    }
}

//  Функция выводит строки
void output(set p, const char *str, const char *filename) {
    if ((p.nf > 1) && (p.h == 0))
        printf("%s:", filename);
    if (p.n == 1)
        printf("%d:", p.sn);
    printf("%s\n", str);
}
