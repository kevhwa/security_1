#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "Checks a client using the intermediate CA cert instead of root CA cert"

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client4.com.cert.pem -key $HOMEINTER/private/www.example_client4.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEINTER/certs/intermediate.cert.pem -prexit -quiet
