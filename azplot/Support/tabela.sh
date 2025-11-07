distances="$(seq -s "" -f "%8.2f " 0 10 100) $(seq   -f "%.2f " 1 95 | awk '{printf "%8.2f ",$1*111.2}')"

i=0
for d in $distances
do
    printf "%8.2f, " $d
    i=$(( i + 1 ))
    [ $(( $i % 10)) -eq 0 ] && echo ""
done

echo ""
echo ""

i=0
for d in $distances
do
    tp=$(taup_time -km $d -time -ph p,Pg,Pn,P -h 0.0 2>&1 | awk -v d="$d" 'NR == 1 {printf "%8.4f",$1}')
    echo -n "$tp, "
    i=$(( i + 1 ))
    [ $(( $i % 10)) -eq 0 ] && echo ""
done

echo ""
echo ""

i=0
for d in $distances
do
    ts=$(taup_time -km $d -time -ph s,Sg,Sn,S -h 0.0 2>&1 | awk -v d="$d" 'NR == 1 {printf "%9.4f",$1}')
    echo -n "$ts, "
    i=$(( i + 1 ))
    [ $(( $i % 10)) -eq 0 ] && echo ""
done

echo "Total de $i pontos"
