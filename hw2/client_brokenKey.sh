#~/bin/bash

HOME=/home/mushu/security_1/hw2
HOMEROOT=$HOME/ca
HOMEINTER=$HOMEROOT/intermediate

PORT=8000
DEPTH=4

echo "***"
echo "Testing client whose key has been tampered with"
echo " "

printf 'GET /file.txt HTTP/1.0\r\n' > $HOME/temp1.txt
sed -e '0,/a/ s/a/A/' <$HOMEINTER/private/www.example_client.com.key.pem > $HOMEINTER/private/www.example_client_brokenkey.com.key.pem

chmod 444 $HOMEINTER/private/www.example_client_brokenkey.com.key.pem

openssl s_client -connect localhost:$PORT -cert $HOMEINTER/certs/www.example_client.com.cert.pem -cert_chain $HOMEINTER/certs/ca-chain.cert.pem -key $HOMEINTER/private/www.example_client_brokenkey.com.key.pem -pass pass:pass -verify $DEPTH -verify_return_error -CApath $HOMEROOT/certs -CAfile $HOMEROOT/certs/ca.cert.pem -prexit -quiet -ign_eof < temp1.txt >> $HOME/clientlog.txt 2>&1

