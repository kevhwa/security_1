#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo " "
echo "Testing self-signed certificate"
echo " "

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client6.com.cert.pem -key $HOMEINTER/private/www.example_client6.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca.cert.pem -prexit -quiet

