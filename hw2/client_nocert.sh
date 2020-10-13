#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Client doesn't send certificate"
echo " "

openssl s_client -connect localhost:$PORT -key $HOMEINTER/private/www.example_client.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca.cert.pem -prexit -quiet >> $HOME/clientlog.txt 2>&1

