#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "Testing client with .com email"
echo " "

printf 'GET /file.txt HTTP/1.0\r\n' > $HOME/exampleRequest.txt

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client_comemail.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain_comemail.cert.pem -key $HOMEINTER/private/www.example_client_comemail.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca_comemail.cert.pem -prexit -quiet -ign_eof < exampleRequest.txt
