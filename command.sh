gcc -c echo.c -I./ 
gcc -c ps.c -I./
gcc main.c -I./ ps.o echo.o
./a.out
