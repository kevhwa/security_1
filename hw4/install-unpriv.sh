#~/bin/bash
dir="$1"
 
[ $# -eq 0 ] && { echo "Usage: $0 dir-name"; exit 1; }
 
if [ -d "$dir" -a ! -h "$dir" ]
then
   echo "Error: $dir already exists in $(readlink -f ${dir})."

else

   mkdir "$1"

   #make sub directories
   cd "$1"
   mkdir bin
   mkdir mail
   mkdir tmp

   cd ../
   cp -r inputs "$1"/

   cd "$1"
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

fi
