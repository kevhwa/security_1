#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Testing conflicted expired certificate but specified 100 days option"
echo " "

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client3.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain.cert.pem -key $HOMEINTER/private/www.example_client3.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca.cert.pem -prexit -quiet  >> $HOME/clientlog.txt 2>&1

