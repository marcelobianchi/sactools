#!/bin/bash 

ps=plot.ps

sac2gmt -i gcarc -i baz -i a -d tele  -n -s gcarc > ttele.txt
sac2gmt -i gcarc -i baz -i a -d tele -r f -n -s gcarc > ttele2.txt
########################################'ONE
t1=-05
y1=0
y2=`sachinfo -d tele gcarc | wc -l | awk '{print $1 + 1}'`

extraWiggle="--HEADER_FONT_SIZE=14 --LABEL_FONT_SIZE=14 --HEADER_OFFSET=0.01c"

cat /dev/null  | psxy -R0/1/0/1 -JX12/20 -Xc -Yc -K -P  > $ps

#Origin time aligned
t2=34.999
pswiggle ttele.txt -m -Z5 -W1p -R$t1/$t2/$y1/$y2 -JX5/20 -B1a5:"Time (s)":/0:"Distance"::."Aligned by Origin":SWne -O -K $extraWiggle >> $ps
awk '$1 == ">" {print $5,$2}' ttele.txt | psxy -R -J -O -Ss.2 -Gred -K >> $ps

# Custom pick 'f' aligned
t2=14.999
pswiggle ttele2.txt -m -Z5 -W1p -R$t1/$t2/$y1/$y2 -JX5/20 -X5.5 -O -B1a5:"Time (s)":/0:"Distance"::."Aligned by f-pick":wSne -K $extraWiggle >> $ps
awk '$1 == ">" {print $5,$2}' ttele2.txt | psxy -R -J -O -Ss.2 -Gred -K >> $ps

d1=47
d2=52
az1=322
az2=328

awk '$1 == ">" {print $3,$2}' ttele.txt | sort -k 2 -n | psxy -R$d1/$d2/$y1/$y2   -JX1.5/20 -W0.5p      -B0/0wsne   -O -K -X5.5 >> $ps
awk '$1 == ">" {print $4,$2}' ttele.txt | sort -k 2 -n | psxy -R$az1/$az2/$y1/$y2 -J        -W0.5p,red              -O -K      >> $ps

anotDistance='--ANNOT_FONT_SIZE_PRIMARY=11 --LABEL_FONT_SIZE=10 --LABEL_OFFSET=0.01c'
anotBaz='--ANNOT_FONT_SIZE_PRIMARY=11 --BASEMAP_FRAME_RGB=red --TICK_PEN=.5p,red --LABEL_FONT_SIZE=10 --LABEL_OFFSET=0.01c'
psbasemap -R$d1/$d2/$y1/$y2   -J -B2:"Distance (deg)":S -O -K -Y-.2  $anotDistance >> $ps
psbasemap -R$az1/$az2/$y1/$y2 -J -B3:"Azimuth (deg)":S  -O -Y-1.5 $anotBaz >> $ps

gv $ps
rm ttele.txt ttele2.txt

