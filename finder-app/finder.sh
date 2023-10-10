#!/bin/bash

# If is anything other than 2 args, exit
if [ $# -ne 2 ] 
then
  echo "Incorrect # of arguments! Arg#=$#"
  echo "Arguments should be: 1) File Directory Path 2) String to search for in directory"
  exit 1
fi

# Check if filesdir is not a directory, exit
if ! [ -d $1 ]
then 
  echo "First argument is not a directory! Arg=$1"
  exit 1
fi

# Count the number of file occurrences for searched string in all directory and subdirectories
X="$(grep -r -l $2 $1 | wc -l)"

# Count the number of occurences for searched string in all directory and subdirectories
Y="$(grep -r $2 $1 | wc -l)"
echo "The number of files are $X and the number of matching lines are $Y" 
