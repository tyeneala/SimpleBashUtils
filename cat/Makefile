NAME = "s21_cat"
CC = gcc
CFLAGS = -c -Wall -Wextra -Werror -std=c11

COV = --coverage

SOURCE = s21_cat.c
OBJECT = $(SOURCE: .c=.o)
EXEFILE = ./s21_cat

all: s21_cat

s21_cat: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE)
	$(CC) $(OBJECT) -o $(EXEFILE)

test:
	$(CC) $(COV) $(CFLAGS) $(SOURCE)
	$(CC) $(COV) $(OBJECT) -o $(EXEFILE)
	sh ../tests/test_cat.sh

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
