#!/bin/sh

test_dir="`dirname $0`/*"
for file in $test_dir
do
  if test -f "$file"; then
    if [[ -x "$file" && ${file: -3} != ".sh" ]]; then
      $file --gtest_output=xml:$file.xml
    fi
  fi
done
