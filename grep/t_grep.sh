#!/bin/bash

COUNTER_SUCCESS=0
COUNTER_FAIL=0
VALG=0
VALG_ERR=0
DIFF_RES=""
GREP=./s21_grep
S21_GREP=s21_grep.txt
GREP_OLD=grep.txt
FILE1=s21_grep.c
FILE2=s21_grep.h
FILE3=Makefile
PATTERN=pattern
LOG=log.txt
rm -f $LOG
rm -r -f val
mkdir -p val
LEAK_CHECK="valgrind --leak-check=yes --show-leak-kinds=all --track-origins=yes --verbose --log-file="

diff_f(){
    VAL="val/val_report_$VALG.txt"
    (( VALG++ ))
    LEAK_CHECK1="$LEAK_CHECK$VAL"
     $LEAK_CHECK1 $GREP $TEST1 > $S21_GREP
    err=$(grep $VAL -e "ERROR SUMMARY:")
    # echo "$err"
    n=$(expr index "$err" "ERROR SUMMARY:")+15
    nn=${err:n:1}

    grep $TEST1 > $GREP_OLD
    DIFF_RES="$(diff -s $S21_GREP $GREP_OLD)"
    if [ "$DIFF_RES" == "Files $S21_GREP and $GREP_OLD are identical" ]
    then
        (( COUNTER_SUCCESS++ ))
    else
        (( COUNTER_FAIL++ ))
        echo "$TEST1" >> $LOG
    fi
    rm $S21_GREP $GREP_OLD


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
            echo -n "$del1"
        else
            echo -n  "."
        fi
    fi
}

for var in v c l n h o
do
    TEST1="for $FILE1  $FILE2 $FILE3 -$var"
    diff_f
    TEST1="for $FILE1 -$var"
    diff_f
    TEST1="-e for -e ^int $FILE1 $FILE2 $FILE3 -$var"
    diff_f
    TEST1="-e for -e ^int $FILE1 -$var"
    diff_f
    TEST1="-e regex -e ^print $FILE1 -$var -f $PATTERN"
    diff_f
    TEST1="-e while -e void $FILE1 $FILE3 -$var -f $PATTERN"
    diff_f
    TEST1="-e while -e void $FILE1 $FILE3 -$var$var2$var3 -f $PATTERN"
    diff_f
    TEST1="-e for $FILE1 -e ^int $FILE2 -$var $FILE3"
    diff_f
done

for var in v c l n
do
    for var2 in c l n h
    do
        for var3 in l n h o
        do
            if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ]
            then
                TEST1="for $FILE1 $FILE2 $FILE3 -$var -$var2 -$var3"
                diff_f
                TEST1="for $FILE1 -$var -$var2 -$var3"
                diff_f
                TEST1="-e for -e ^int $FILE1 $FILE2 $FILE3 -$var -$var2 -$var3"
                diff_f
                TEST1="-e for -e ^int $FILE1 -$var -$var2 -$var3"
                diff_f
                TEST1="-e regex -e ^print $FILE1 -$var -$var2 -$var3 -f $PATTERN"
                diff_f
                TEST1="-e while -e void $FILE1 $FILE3 -$var -$var2 -$var3 -f $PATTERN"
                diff_f
                TEST1="-e while -e void $FILE1 $FILE3 -$var$var2$var3 -f $PATTERN"
                diff_f
                TEST1="-e for $FILE1 -e ^int $FILE2 -$var $FILE3 -$var2 -$var3"
                diff_f
            fi
        done
    done
done

TEST1="for fl.txt $FILE2 $FILE3"
diff_f
TEST1="for fl.txt $FILE2 $FILE3 -s"
diff_f
TEST1="for $PATTERN $FILE2 $FILE3 -s -f f2.txt"
diff_f
TEST1="for $PATTERN $FILE2 $FILE3 -f f2.txt"
diff_f

echo -e "\nSUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"
echo "VALGRIND ERROR: $VALG_ERR"

if [[ "$VALG_ERR" == "$NL" ]]
then
    rm -r val
fi
