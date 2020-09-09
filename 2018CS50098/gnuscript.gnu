set title "Average amount of time a customer spent in bank versus Number of tellers"
set ylabel "Time (minutes)"
set yrange [0:]
set xlabel "Number of tellers"
set xtics out nomirror
set boxwidth 0.5
set style fill solid
set terminal png size 1180,630
set output "./output/plot.png"
plot "./output/data.txt" using 1:2:xtic(1) with boxes lc rgb 22851769 title "Common queue for all tellers"
