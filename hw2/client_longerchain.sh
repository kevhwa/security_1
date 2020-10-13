#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Client has longer certificate chain"
echo " "

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client_longerchain.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain2.cert.pem -key $HOMEINTER/private/www.example_client_longerchain.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca.cert.pem -prexit -quiet >> $HOME/clientlog.txt 2>&1

