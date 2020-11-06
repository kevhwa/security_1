#~/bin/bash

for i in inputs/*

do
        echo "***testing: $i***"
        bin/mail-in <$i
        echo " "
done
