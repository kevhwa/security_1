#~/bin/bash

rm -rf "$1"
mkdir "$1"


#make sub directories
cd "$1"
mkdir bin
mkdir mail
mkdir inputs
mkdir tmp
mkdir lib

cd mail 

(umask 077; mkdir addleness)
(umask 077; mkdir analects)
(umask 077; mkdir annalistic)
(umask 077; mkdir anthropomorphologically)
(umask 077; mkdir blepharosphincterectomy)
(umask 077; mkdir corector)
(umask 077; mkdir durwaun)
(umask 077; mkdir dysphasia)
(umask 077; mkdir encampment)
(umask 077; mkdir endoscopic)
(umask 077; mkdir exilic)
(umask 077; mkdir forfend)
(umask 077; mkdir gorbellied)
(umask 077; mkdir gushiness)
(umask 077; mkdir muermo)
(umask 077; mkdir neckar)
(umask 077; mkdir outmate)
(umask 077; mkdir outroll)
(umask 077; mkdir overrich)
(umask 077; mkdir philosophicotheological)
(umask 077; mkdir pockwood)
(umask 077; mkdir polypose)
(umask 077; mkdir refluxed)
(umask 077; mkdir reinsure)
(umask 077; mkdir repine)
(umask 077; mkdir scerne)
(umask 077; mkdir starshine)
(umask 077; mkdir unauthoritativeness)
(umask 077; mkdir unminced)
(umask 077; mkdir unrosed)
(umask 077; mkdir untranquil)
(umask 077; mkdir urushinic)
(umask 077; mkdir vegetocarbonaceous)
(umask 077; mkdir wamara)
(umask 077; mkdir whaledom)
(umask 077; mkdir temp);

cd ../../

cp tester.sh $1/

cp testcase0 $1/inputs/
cp testcase1 $1/inputs/
cp testcase2 $1/inputs/
cp testInvalidName1 $1/inputs/
cp testInvalidName2 $1/inputs/
cp testInvalidName3 $1/inputs/
cp testInvalidFormat1 $1/inputs/
cp testInvalidFormat2 $1/inputs/
cp testInvalidFormat3 $1/inputs/
cp testInvalidFormat4 $1/inputs/
cp testInvalidFormat5 $1/inputs/
cp testLongStr $1/inputs/
cp testMoreRecv $1/inputs/
cp longfile.txt $1/inputs/
