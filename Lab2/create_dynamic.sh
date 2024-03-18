export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:."
gcc -c -Wall -fPIC -D_GNU_SOURCE function_5.c
gcc -c -Wall -fPIC -D_GNU_SOURCE function_6.c
gcc -c -Wall -fPIC -D_GNU_SOURCE function_7.c
gcc -c -Wall -fPIC -D_GNU_SOURCE function_8.c
gcc function_5.o function_6.o function_7.o function_8.o -shared -o lib_dynamic.so
gcc -c dynamic_test.c -o dynamic_test.o
gcc dynamic_test.o -L. -l_dynamic -o dynamic_test.out 
ldd dynamic_test.out
./dynamic_test.out
