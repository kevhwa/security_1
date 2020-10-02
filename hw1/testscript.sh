#!/bin/bash

make clean
make

printf "\n*******TEST SCRIPT******** adding files river, waterfall, nature and largeText file to archiveTest"
./cstore add -p laksjflkdsajfds archiveTest waterfall.jpeg nature.jpg river.jpg largeText.txt

printf "\n*******TEST SCRIPT******** extracting all files, file names given in different order"
./cstore extract -p laksjflkdsajfds archiveTest river.jpg waterfall.jpeg largeText.txt nature.jpg

printf "\n*******TEST SCRIPT******** extracting all files again, file names given in different order"
./cstore extract -p laksjflkdsajfds archiveTest largeText.txt waterfall.jpeg river.jpg nature.jpg

printf "\n*******TEST SCRIPT******** list should list all file names"
./cstore list archiveTest

printf "\n*******TEST SCRIPT******** deleting river.jpg and waterfall.jpeg from archive"
./cstore delete -p laksjflkdsajfds archiveTest waterfall.jpeg river.jpg

printf "\n*******TEST SCRIPT******** extracting all files again, only largeText.txt and nature.jpg will be extracted"
./cstore extract -p laksjflkdsajfds archiveTest largeText.txt waterfall.jpeg river.jpg nature.jpg

printf "\n*******TEST SCRIPT******** list should list only largeText.txt and nature.jpeg"
./cstore list archiveTest

printf "\n*******TEST SCRIPT******** Trying to deleting river.jpg and waterfall.jpeg again from archive will delete nothing"
./cstore delete -p laksjflkdsajfds archiveTest waterfall.jpeg river.jpg

printf "\n*******TEST SCRIPT******** adding files river and waterfall back to archiveTest"
./cstore add -p laksjflkdsajfds archiveTest waterfall.jpeg river.jpg 

printf "\n*******TEST SCRIPT******** list should list all files again"
./cstore list archiveTest

printf "\n*******TEST SCRIPT******** adding all files to archiveTest again for test"
./cstore add -p laksjflkdsajfds archiveTest waterfall.jpeg nature.jpg river.jpg largeText.txt

printf "\n*******TEST SCRIPT******** adding all files one more time for test"
./cstore add -p laksjflkdsajfds archiveTest waterfall.jpeg nature.jpg river.jpg largeText.txt

printf "\n*******TEST SCRIPT******** Deleting largeText.txt two times as two arguments to show multiple instances of same file can be deleted at same time if they exist in archive"
./cstore delete -p laksjflkdsajfds archiveTest largeText.txt largeText.txt

printf "\n*******TEST SCRIPT******** adding cstore.c file as test"
./cstore add -p laksjflkdsajfds archiveTest cstore.c

printf "\n*******TEST SCRIPT******** extracting cstore.c"
./cstore extract -p laksjflkdsajfds archiveTest cstore.c

printf "\n*******TEST SCRIPT******** adding cstore.c file with wrong password"
./cstore add -p laksjflkds3232ajfds archiveTest cstore.c

printf "\n*******TEST SCRIPT******** extracting cstore.c file with wrong password"
./cstore delete -p laksjflkds3232ajfds archiveTest cstore.c

printf "\n*******TEST SCRIPT******** deleting cstore.c file with wrong password"
./cstore extract -p laksjflkds3232ajfds archiveTest cstore.c

printf "\n*******TEST SCRIPT******** adding random file that doesnt exist"
./cstore add -p laksjflkdsajfds archiveTest cstorasdfdsaffsadfdafadfdsafe.c

printf "\n*******TEST SCRIPT******** extracting random file that doesnt exist"
./cstore extract -p laksjflkdsajfds archiveTest cstorasdfdsaffsadfdafadfdsafe.c

printf "\n*******TEST SCRIPT******** deleting random file that doesnt exist"
./cstore delete -p laksjflkdsajfds archiveTest cstorasdfdsaffsadfdafadfdsafe.c

printf "\n*******TEST SCRIPT******** can vim decrypted cstore.c and verity all is good, along with running command cmp <file> <decryptedfile>"

printf "\n*******TEST SCRIPT******** test script done"

exit 0
