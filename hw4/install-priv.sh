#~/bin/bash
dir="$1"
 
[ $# -eq 0 ] && { echo "Usage: $0 dir-name"; exit 1; }
 
if [ -d "$dir" -a ! -h "$dir" ]
then
    cd "$1"

    cd mail 

    chown addleness addleness
    chown analects analects
    chown annalistic annalistic
    chown anthropomorphologically anthropomorphologically
    chown blepharosphincterectomy blepharosphincterectomy
    chown corector corector
    chown durwaun durwaun
    chown dysphasia dysphasia
    chown encampment encampment
    chown endoscopic endoscopic
    chown exilic exilic
    chown forfend forfend
    chown gorbellied gorbellied
    chown gushiness gushiness
    chown muermo muermo
    chown neckar neckar
    chown outmate outmate
    chown outroll outroll
    chown overrich overrich
    chown philosophicotheological philosophicotheological
    chown pockwood pockwood
    chown polypose polypose
    chown refluxed refluxed
    chown reinsure reinsure
    chown repine repine
    chown scerne scerne
    chown starshine starshine
    chown unauthoritativeness unauthoritativeness
    chown unminced unminced
    chown unrosed unrosed
    chown untranquil untranquil
    chown urushinic urushinic
    chown vegetocarbonaceous vegetocarbonaceous
    chown wamara wamara
    chown whaledom whaledom

    cd ../bin

    chmod 700 mail-out
    chown root mail-out

    chown root mail-in
    chmod u+s mail-in

    cd ../

    chmod 700 tmp
    chown root tmp

else

    echo "Error: $dir does not exist."
fi