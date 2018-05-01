g++ test.cpp 
./a.out 2 5000 3421 generated
mpirun -np 4 ronak generated out_ronak 2 5000
mpirun -np 4 sort2d generated out_saket 2 5000
diff out_ronak out_saket
