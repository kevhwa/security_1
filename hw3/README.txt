Uni: jch2169
Hw: hw3


Directions: 
- make will create the executables in current directory
- make clean will delete by products of make in current directory
- make install DEST=($DEST) will copy the executables into the provided path

- ./create-tree.sh <NAME> will create the tree directory. It will also place
  the test script and test files into the appropriate place

1. create tree
2. make install


Mail-in summary

- Reads in to check MAIL FROM (case insensitive). If it fails, it will skip
  to the next message if available
- Reads in to check RCPT TO (case insensitive). If it fails, it will skip to
  the next viable RCPT TO. If no valid RCPT TO is found, it will skip this
  message. Otherwise, I store the correctly formatted users into an array.
- Read in from std in and write out to a temp file
- For each correctly formatted user, I fork, read from the temp file and
  read out to the pipe.
- Once I'm done, I wait for the child to finish and for any error codes. 
- Loop again if there is more to read


Mail-out summary

- First check if the recipient passed as an arg is one thats valid
  - if yes, go on. If not, exit with exit code -1
- Read in from stdin and write out to the correct mailbox


Specific behaviors
- Since SMTP protocol specifies that the max possible path length is 256
  bytes, I check to see that the sender/receivers names are greater than
  256. If it is, I will skip that and go to the next possible one. 
- On piazza, the professor said that the max input file size is 1gb. Since
  I'm not sure what the cloud vm can handle, I assume that it can handle 1gb
  of space. The only time this will be relevant for me is if the vm can't
  actually write 1gb to a file in one sitting
  - The program will never maloc more space that the number of valid
    unique recipients. My max buffer size is also set to 500 and never holds
    more than that in memory. 
  - I also only read in 500 bytes at a time so there shouldn't be any stack
    memory space issues

- MAIL FROM:<name> , RCPT TO:<name>
  - Any issues with this header, whether it be invalid username, misspelled 'maile
    from' or missing < >, all leads to skipping to the next message
- Any order mixups, MAIL FROM, MAIL FROM will also lead to skipping to the
  next message

