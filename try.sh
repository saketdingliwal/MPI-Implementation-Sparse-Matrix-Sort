g++ test.cpp 
./a.out 452 4521 34215 generated
mpirun -np 4 ronak generated out_ronak 452 4521
mpirun -np 4 sort2d generated out_saket 452 4521
diff out_ronak out_saket
