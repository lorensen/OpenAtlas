#!/bin/bash 
all=`ls -1 VTK| sort | cut -d":" -f2 | cut -d"." -f1,1`
echo $all

for i in $all; do
/home/lorensen/ProjectsGIT/OpenAtlas/build/DisplayAdjacentModels $i /home/lorensen/ProjectsGIT/OpenAtlas/Data/SPLAtlas/hncma-atlas-lut.ctbl /home/lorensen/ProjectsGIT/OpenAtlas/Models/SPLAtlas/adjacencies.txt 
done
