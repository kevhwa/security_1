#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

openssl s_server -accept $PORT -cert $HOMEINTER/certs/www.example_server.com.cert.pem -key $HOMEINTER/private/www.example_server.com.key.pem -pass pass:pass -Verify $DEPTH -CAfile $HOMEINTER/certs/ca-chain.cert.pem -HTTP 
