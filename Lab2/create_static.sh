gcc -c function_1.c -o function_1.o
gcc -c function_2.c -o function_2.o
gcc -c function_3.c -o function_3.o
gcc -c function_4.c -o function_4.o
ar r lib_static.a function_1.o function_2.o function_3.o function_4.o 
ar tv lib_static.a
gcc -c static_test.c -o static_test.o
gcc static_test.o lib_static.a -o static_test.out
./static_test.out
