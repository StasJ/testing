#!/bin/bash

FILES="
/glade/scratch/mininni/part/TG1024box/outs/xlg.00000100.lag
"
PATH=/glade/scratch/mininni/part/TG1024box/outs

COUNTER=2

#for f in $FILES
while [ $COUNTER -lt 3682 ];
do
    printf -v idx "%04d" $COUNTER
    FILE=$PATH/xlg.0000$idx.lag
    echo "working on file $FILE"
    /glade/u/home/shaomeng/Git/VAPOR-density/apps/voronoi-density/pgi_bin/v5 $FILE /glade/scratch/shaomeng/pablo_density/$COUNTER.den
    COUNTER=$((COUNTER+4))
done
