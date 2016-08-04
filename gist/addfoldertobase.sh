#!/bin/bash/

dossierBase=$PWD

cd $1

liste_fichiers=`ls`

cd $dossierBase

for fichier in $liste_fichiers
do
./compute_gist -nblocks 2 -addtobase $1/$fichier
done

cd $1
rm *.ppm
