#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
VALG=0
VALG_ERR=0
DIFF_RES=""
TEST_FILE="bytes.txt"
LOG=log.txt
rm -f $LOG
CAT=./s21_cat
S21_CAT=s21_cat.txt
CAT_OLD=cat.txt

FILE1=s21_grep.c
FILE2=s21_grep.h
FILE3=Makefile
PATTERN=pattern

rm -r -f val
mkdir -p val
LEAK_CHECK="valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes --verbose --log-file="

diff_f(){
    VAL="val/val_report_$VALG.txt"
    (( VALG++ ))
    LEAK_CHECK1="$LEAK_CHECK$VAL"
    $LEAK_CHECK1 $CAT $TEST1 > $S21_CAT
    err=$(grep $VAL -e "ERROR SUMMARY:")
    # echo "$err"
    n=$(expr index "$err" "ERROR SUMMARY:")+15
    nn=${err:n:1}

    cat $TEST1 > $CAT_OLD
    DIFF_RES="$(diff -s $S21_CAT $CAT_OLD)"
    if [ "$DIFF_RES" == "Files $S21_CAT and $CAT_OLD are identical" ]
    then
        (( COUNTER_SUCCESS++ ))
    else
        echo "$TEST1" >> $LOG
        (( COUNTER_FAIL++ ))
    fi
    rm $S21_CAT $CAT_OLD

    NL=0
    if [[ "$nn" > "$NL" ]]
    then
        (( VALG_ERR++ ))
        echo -n "E"
    else
        rm $VAL
        (( VALG-- ))
        del=$((($COUNTER_SUCCESS+$COUNTER_FAIL)%100))
        del1=$((($COUNTER_SUCCESS+$COUNTER_FAIL)/100))
        if [[ "$del" == "$NL" ]]
        then
            echo -n -e "\n$del1"
        else
            echo -n  "."
        fi
    fi
}

for var in b e n s t v #E T -number-nonblank -number -squeeze-blank
do
    TEST1="-$var $TEST_FILE"
    diff_f
done

for var in b e n s t #E T -number-nonblank -number -squeeze-blank
do
  for var2 in e n s t v #E T -number-nonblank -number -squeeze-blank
  do
    if [ $var != $var2 ]
    then
        TEST1="-$var -$var2 $TEST_FILE"
        diff_f
    fi
  done
done

for var in b e n s #E T -number-nonblank -number -squeeze-blank
do
    for var2 in e n s t #E T -number-nonblank -number -squeeze-blank
    do
      for var3 in n s t v #E T -number-nonblank -number -squeeze-blank
      do
        if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ]
        then
            TEST1="-$var -$var2 -$var3 $TEST_FILE"
            diff_f
        fi
      done
  done
done

for var in b e n #E T -number-nonblank -number -squeeze-blank
do
  for var2 in e n s #E T -number-nonblank -number -squeeze-blank
  do
      for var3 in n s t #E T -number-nonblank -number -squeeze-blank
      do
          for var4 in s t v #E T -number-nonblank -number -squeeze-blank
          do
            if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ] && [ $var != $var4 ] && [ $var2 != $var4 ] && [ $var3 != $var4 ]
            then
                TEST1="-$var -$var2 -$var3 -$var4 $TEST_FILE"
                diff_f
            fi
          done
      done
  done
done

for var in b e n #E T -number-nonblank -number -squeeze-blank
do
  for var2 in e n s #E T -number-nonblank -number -squeeze-blank
  do
      for var3 in n s t #E T -number-nonblank -number -squeeze-blank
      do
          for var4 in t v #E T -number-nonblank -number -squeeze-blank
          do
            if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ] && [ $var != $var4 ] && [ $var2 != $var4 ] && [ $var3 != $var4 ]
            then
                TEST1="-$var$var2$var3$var4 $TEST_FILE"
                diff_f
            fi
          done
      done
  done
done

echo -e "\nSUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
echo "VALGRIND ERROR: $VALG_ERR"

if (( "$VALG_ERR" == 0 ))
then
    rm -r val
fi
