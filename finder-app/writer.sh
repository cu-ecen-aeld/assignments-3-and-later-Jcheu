#!/bin/bash

# Get the directory to create the filepath for
directory="$(dirname $1)"

# If is anything other than 2 args, exit
if [ $# -ne 2 ] 
then
  echo "Incorrect # of arguments! Arg#=$#"
  echo "Arguments should be: 1) File path w/ filename 2) String to write"
  exit 1
fi

# Make the directory filepath, explicitly add exit code/error print even though it happens naturally
if ! mkdir -p $directory
then
  echo "Failed to make directory!"
  exit 1
fi

# Write to the file
if ! echo $2 > $1
then
  echo "Failed to write to file!"
  exit 1
fi
