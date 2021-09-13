#!/bin/bash
bb=$1
wd=$(dirname "$0")

printf "[running tests]\n"
for unit in $wd/*.bee; do
  expectations="${unit%.bee}.txt"
  expected_output=$(tail -n $(($(wc -l $expectations | cut -d ' ' -f1) - 1)) $expectations)
  expected_ecode=$(head -n 1 $expectations)
  actual_output=$($bb "$unit")
  actual_ecode=$?

  if [ $actual_ecode -ne $expected_ecode ]; then
    printf "\n$unit:0:0 expected code is $expected_ecode, actual code is $actual_ecode\n"
    printf "expected output is: \n"
    printf "$expected_output\n"
    printf "actual output is: \n"
    printf "$actual_output\n"
    continue
  fi

  if [ "$expected_output" != "$actual_output" ]; then
    printf "\n$unit:0:0 unexpected command output\n"
    printf "==== expected output ====\n"
    printf "$expected_output\n"
    printf "==== actual output ====\n"
    printf "$actual_output\n"
    printf "====\n"
    continue
  fi

  printf "."
done
printf "done\n"
