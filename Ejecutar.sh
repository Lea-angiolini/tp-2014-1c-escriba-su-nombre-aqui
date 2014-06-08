#!/bin/bash

ubicacion=$(pwd)
clear
echo "Ingrese cantidad de CPU (max  8)"
read n

cd $ubicacion
cd UMV/Debug
xterm -T UMV -hold -e "./UMV ../resources/config.cfg" &
sleep 0.5

cd $ubicacion
cd Kernel/Debug
xterm -T Kernel -hold -e "./Kernel ../resources/config.cfg" &
sleep 0.5

cant=1

if [ $n -gt 8 ]; then
   n=8
fi

cd $ubicacion
cd CPU/Debug
while [ $cant -le $n ]; do
	xterm -T CPU$cant -hold -e "./CPU ../resources/config.cfg" &
    	let cant=$cant+1
done


cd $ubicacion
cd Programa/resources
echo "Ingrese script a ejecutar (no hace falta .ansisop)"
echo " "
ls
read s

while [ $s != 0 ]; do
		
		cd $ubicacion
		cd Programa/Debug
		export ANSISOP_CONFIG=../resources/config.cfg
		xterm -T $s -hold -e "./Programa ../resources/$s.ansisop" &
			
		clear
		cd $ubicacion
		cd Programa/resources
		echo "Ingrese script a ejecutar (no hace falta .ansisop)"
		echo " "
		ls
		read s

	done

clear
exit 0

