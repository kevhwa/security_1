#~/bin/bash

HOME=/home/mushu/security_1/hw2

echo "Testing client tests that don't necessarily work"

./client_badident.sh
./client_conflictingexpdate.sh
./client_mismatchkey.sh
./client_mismatch.sh
./client_rejectemail.sh
./client_der.sh
./client_broken.sh
