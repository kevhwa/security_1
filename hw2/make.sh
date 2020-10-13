 #~/bin/bash

 HOME=/home/mushu/security_1/hw2
 HOMEROOT=$HOME/ca
 HOMEINTER=$HOMEROOT/intermediate

 rm -rf $HOME/ca
 rm $HOME/clientlog.txt $HOME/serverlog.txt

 ./CA_bash.sh
 touch clientlog.txt
 touch serverlog.txt


 #ADD CHMOD FOR ALL THE SCRIPTS HERE
