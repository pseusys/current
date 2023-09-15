#!/bin/sh

# Since I don't wanna mess with my home dir, I'll create the file here.

# Create a directory called training_system. In this directory, create another directory called exercise4 and move into this directory.
mkdir -p training_system/exercise4
cd training_system/exercise4

# In this directory, create a file file1.txt where you write a small text.
echo -e "some small text\nsome more" > file1.txt

# Count the number of lines in this file and display the result on the screen.
test $(wc -l < file1.txt) -eq 2

# List the content of your home directory and store the result in a file exercise4/file2.txt
ls ~ > file2.txt

# Verify experimentally how many of the last lines of a file the command tail displays by default. Of course, you should not count manually.
echo -e "Number of lines outputted by 'tail' by default: $(tail file2.txt | wc -l)\n"

# Count the number of lines including the word "warning" in the files with a ".log" extension in directory /var/log
echo -e "Number of 'warning's in logs dir (with errors): $(grep warning /var/log/*.log | wc -l)\n"

# Your previous command might display some error messages in the terminal. How can you get rid of these messages?
echo -e "Number of 'warning's in logs dir (without errors): $(grep warning /var/log/*.log 2> /dev/null | wc -l)\n"

# Still related to question 6 above: how to write the output of your command (the command that lists the lines with the word "warning"), including the error messages, to a file?
grep warning /var/log/*.log 1> file3.txt 2> &1

echo Execution finiished!
