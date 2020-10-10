#~/bin/bash

HOME=/home/mushu/security_1/hw2
#
# Creating Root CA
#

mkdir $HOME/ca
HOMEROOT=$HOME/ca

#cd ca
mkdir $HOMEROOT/certs $HOMEROOT/crl $HOMEROOT/newcerts $HOMEROOT/private
chmod 700 $HOMEROOT/private
touch $HOMEROOT/index.txt
echo 1000 > $HOMEROOT/serial

openssl genrsa -aes256 -out $HOMEROOT/private/ca.key.pem 4096

chmod 400 $HOMEROOT/private/ca.key.pem
cp $HOME/root_openssl.cnf $HOMEROOT/root_openssl.cnf

openssl req -config $HOMEROOT/root_openssl.cnf -key $HOMEROOT/private/ca.key.pem -new -x509 -days 7300 -sha256 -extensions v3_ca -out $HOMEROOT/certs/ca.cert.pem

chmod 444 $HOMEROOT/certs/ca.cert.pem

#
# Creating Intermediary CA
#

mkdir $HOME/ca/intermediate
HOMEINTER=$HOMEROOT/intermediate

mkdir $HOMEINTER/certs $HOMEINTER/crl $HOMEINTER/newcerts $HOMEINTER/private
chmod 700 $HOMEINTER/private
touch $HOMEINTER/index.txt
echo 1000 > $HOMEINTER/serial

cp $HOME/inter_openssl.cnf $HOMEINTER/inter_openssl.cnf

openssl genrsa -aes256 -out $HOMEINTER/private/intermediate.key.pem

chmod 400 $HOMEINTER/private/intermediate.key.pem

#Create intermediary certficate request to become intermediate CA
openssl req -config $HOMEINTER/inter_openssl.cnf -new -sha256 -key $HOMEINTER/private/intermediate.key.pem -out $HOMEINTER/csr/intermediate.csr.pem

#CA root now signs intermediate certificate request to create intermediate CA
openssl ca -config $HOMEROOT/root_openssl.cnf -extensions v3_intermediate_ca -days 3650 -notext -md sha256 -in $HOMEINTER/csr/intermediate.csr.pem -out $HOMEINTER/certs/intermediate.cert.pem

chmod 444 $HOMEINTER/certs/intermediate.cert.pem

