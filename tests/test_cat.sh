#!/bin/zsh
clear

cd ../tests

data="./data_test_cat.txt"
nafi=../cat/s21_cat
sco_t=$(wc -l $data | awk '{print $1}')
SUCCESS=0
FAIL=0

for (( i = 1; i <= $sco_t; i++ ))
do
    echo "\033[32m Test:" $i "\033[0m"
    coli=$(head -n $i $data | tail -n 1)
    echo "\033[33m" $coli "\033[0m"
    cat $coli > test1.txt
    $nafi $coli > test2.txt
    
    echo $(diff -s test1.txt test2.txt)
    
    if [[ $(cmp test1.txt test2.txt) ]]
    then
        echo "\033[34m St:" $(cat $coli) "\033[0m"
        echo "\033[35m My:" $($nafi $coli) "\033[0m"
        (( FAIL++ ))
    else
        (( SUCCESS++ ))
    fi
#    rm test1.txt test2.txt
done

#echo "*** Press twice Ctrl + D ***"
#cat - | echo qwerty > test1.txt
#$nafi - | echo qwerty > test2.txt
#echo $(diff -s test1.txt test2.txt)
#
#echo "*** (Input text and press twice Ctrl + D) x 2 ***"
#cat > test1.txt
#$nafi > test2.txt
#echo $(diff -s test1.txt test2.txt)

echo "\033[36m *** Total: " $sco_t "\tSuccess: " $SUCCESS "\tFail: " $FAIL " ***\033[0m"

cd ../cat
