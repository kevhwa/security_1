 #~/bin/bash

 HOME=/home/mushu/security_1/hw2
 HOMEROOT=$HOME/ca
 HOMEINTER=$HOMEROOT/intermediate

 rm -rf $HOME/ca
 rm $HOME/clientlog.txt $HOME/serverlog.txt
 rm $HOME/temp1.txt $HOME/temp2.txt $HOME/temp3.txt


 ./CA_bash.sh
 touch $HOME/clientlog.txt
 touch $HOME/serverlog.txt


 #ADD CHMOD FOR ALL THE SCRIPTS HERE