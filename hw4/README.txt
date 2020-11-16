Directions: 

1. make install DEST=<dir-name>

- make install DEST=($DEST) will copy the executables into the provided path
- make will create the executables in current directory
- make clean will delete by products of make in current directory

Security architecture
a. Each mailbox is owned by each user and only that specific user has full access. No group or others have permissions.
b. Mail-in is executable by anyone and is a privileged program. 
c. Mail-out is owned by root and thus can only be run by root (in this case, mail-in). Mail-out does not have any privileges 
  but rather inherits from mail-in.
