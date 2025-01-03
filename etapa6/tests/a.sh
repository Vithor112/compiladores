make;
./etapa6 < e.txt > e.s;
gcc e.s -o e;
./e;
echo $?;

