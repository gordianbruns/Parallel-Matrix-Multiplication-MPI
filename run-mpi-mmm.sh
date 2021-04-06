# for each version


# serial, 1 thread, i = 100-1000, r = 10, p = w0
echo w0
echo serial
for items in 100 200 300 400 500 600 700 800 900 1000; do echo; echo $items; (./mmm-naive -i $items -r 10 -p w0) >> results.txt; done

# strong scaling, np = 2-16, i = 1,000, r = 10, p = w0
echo
echo strong scaling
for np in 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do echo; echo $np; (mpirun -np $np --mca btl_tcp_if_include 192.168.5.0/24 -hostfile whedon-hosts mpi_mmm -i 1000 -r 10 -p w0) >> results.txt; done

# weak scaling, np = 2-16, i = np * 100, r = 10, p = w0
echo
echo weak scaling
for np in 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16; do echo; echo $np; (mpirun -np $np --mca btl_tcp_if_include 192.168.5.0/24 -hostfile whedon-hosts mpi_mmm -i `expr $np \* 100` -r 10 -p w0) >> results.txt; done
