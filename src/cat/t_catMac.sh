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

diff_f(){
    $CAT $TEST1 > $S21_CAT
    cat $TEST1 > $CAT_OLD
    DIFF_RES="$(diff -s $S21_CAT $CAT_OLD)"
    if [ "$DIFF_RES" == "Files $S21_CAT and $CAT_OLD are identical" ]
    then
        (( COUNTER_SUCCESS++ ))
    else
        echo -n "E"
        echo "$TEST1" >> $LOG
        (( COUNTER_FAIL++ ))
    fi
    rm $S21_CAT $CAT_OLD

    NL=0
    del=$((($COUNTER_SUCCESS+$COUNTER_FAIL)%100))
    del1=$((($COUNTER_SUCCESS+$COUNTER_FAIL)/100))
    if [[ "$nn" > "$NL" ]]
    then
        echo -n -e "\n$del1"
    else
        echo -n  "."
    fi
}

for var in b e n s t v
do
    TEST1="-$var $TEST_FILE"
    diff_f
done

for var in b e n s t
do
  for var2 in e n s t v
  do
    if [ $var != $var2 ]
    then
        TEST1="-$var -$var2 $TEST_FILE"
        diff_f
    fi
  done
done

for var in b e n s
do
    for var2 in e n s t
    do
      for var3 in n s t v
      do
        if [ $var != $var2 ] && [ $var2 != $var3 ] && [ $var != $var3 ]
        then
            TEST1="-$var -$var2 -$var3 $TEST_FILE"
            diff_f
        fi
      done
  done
done

for var in b e n
do
  for var2 in e n s
  do
      for var3 in n s t
      do
          for var4 in s t v
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

for var in b e n
do
  for var2 in e n s
  do
      for var3 in n s t
      do
          for var4 in t v
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
