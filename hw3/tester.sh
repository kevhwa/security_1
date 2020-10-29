#~/bin/bash

for i in inputs/*

do
        bin/mail-in <$i
done
