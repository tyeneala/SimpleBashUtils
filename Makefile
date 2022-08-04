NAME = "s21_grep"
CC = gcc
CFLAGS = -c -Wall -Wextra -Werror -std=c11

COV = --coverage

SOURCE = s21_grep.c
OBJECT = $(SOURCE: .c=.o)
EXEFILE = ./s21_grep

all: s21_grep

s21_grep: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE)
	$(CC) $(OBJECT) -o $(EXEFILE)

test:
	$(CC) $(COV) $(CFLAGS) $(SOURCE)
	$(CC) $(COV) $(OBJECT) -o $(EXEFILE)
	sh ../tests/test_grep.sh

report:
	gcov $(SOURCE) -m -k -f
	lcov -t $(NAME) -o $(NAME).info -c -d .
	genhtml -o report $(NAME).info
	open ./report/index.html

clean:
	rm -rf *.o *.cfg *.gcno *.gcda *.out *.dSYM *.gcov *.info *.a
	rm -rf ./report
	rm -rf $(EXEFILE)

rebuild: clean all
	  
cpp:
	cppcheck $(SOURCE)
	
lint:
	cp ../../materials/linters/CPPLINT.cfg ./
	python3 ../../materials/linters/cpplint.py --extensions=c $(SOURCE)
