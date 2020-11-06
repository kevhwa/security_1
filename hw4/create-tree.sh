#~/bin/bash


rm -rf "$1"
mkdir "$1"

#make the sub directories
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
mkdir forfend
mkdir gorbellied
mkdir gushiness
mkdir muermo
mkdir neckar
mkdir outmate
mkdir outroll
mkdir overrich
mkdir philosophicotheological
mkdir pockwood
mkdir polypose
mkdir refluxed
mkdir reinsure
mkdir repine
 (umask 077; mkdir scerne)
mkdir starshine
mkdir unauthoritativeness
mkdir unminced
mkdir unrosed
mkdir untranquil
mkdir urushinic
mkdir vegetocarbonaceous
mkdir wamara
mkdir whaledom

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

