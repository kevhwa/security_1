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

mkdir addleness
mkdir analects
mkdir annalistic
mkdir anthropomorphologically
mkdir blepharosphincterectomy
mkdir corector
mkdir durwaun
mkdir dysphasia
mkdir encampment
mkdir endoscopic
mkdir exilic
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
mkdir scerne
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

