#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "Testing client that cant auth server"
echo " "

printf 'GET /file.txt HTTP/1.0\r\n' > $HOME/exampleRequest.txt

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client_noauth.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain.cert.pem -key $HOMEINTER/private/www.example_client_noauth.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca.cert.pem -prexit -quiet -ign_eof < exampleRequest.txt