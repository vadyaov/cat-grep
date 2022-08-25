#!/bin/bash

for flag in -iv -vn -ce -no -ns
do
    echo "TEST $flag"
    ./s21_grep $flag them pattern.txt >s21_grep_result
    grep $flag them pattern.txt >grep_result
    diff -s s21_grep_result grep_result
done

rm -rf s21_grep_result grep_result

for flag in -i -v -c -l -n
do
    echo "TEST $flag"
    ./s21_grep $flag -e "abc" pattern.txt >s21_grep_result
    grep $flag -e "abc" pattern.txt >grep_result
    diff -s s21_grep_result grep_result
done

rm -rf s21_grep_result grep_result

for flag in -iv -lv -in -vl -cn -lc -vo -si -os -nf -lf
do
    echo "TEST $flag"
    ./s21_grep -s -h $flag pattern.txt -e "test" file.txt pattern.txt > s21_grep_result
    grep -s -h $flag pattern.txt -e "test" file.txt pattern.txt > grep_result
    diff -s s21_grep_result grep_result
done

rm -rf s21_grep_result grep_result