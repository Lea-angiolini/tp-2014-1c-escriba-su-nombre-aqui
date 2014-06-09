#!/bin/bash

ubicacion=$(pwd)
clear
echo "Ingrese cantidad de CPU "
read n
clear

cd $ubicacion
cd UMV/Debug
xterm -T UMV -hold -geometry "120x25" -e "./UMV ../resources/config.cfg" &
sleep 0.1

cd $ubicacion
cd Kernel/Debug
xterm -T Kernel -hold -geometry "120x25" -e "./Kernel ../resources/config.cfg" &
sleep 0.2

cant=1

cd $ubicacion
cd CPU/Debug

while [ $cant -le $n ]; do
		xterm -T CPU$cant -hold -geometry "120x30" -e "./CPU ../resources/config.cfg" &
    		let cant=$cant+1
done


cd $ubicacion
cd Programa/resources
echo "Ingrese script a ejecutar (no hace falta .ansisop).  Ingrese '1' para agregar una CPU. Ingrese '0' para salir"
echo " "
ls
read s

while [ $s != 0 ]; do
		
		if [ $s = 1 ]; then
			cd $ubicacion
			cd CPU/Debug
			xterm -T CPU$cant -hold -geometry "120x30" -e "./CPU ../resources/config.cfg" &
    			let cant=$cant+1
		else

			cd $ubicacion
			cd Programa/Debug
			export ANSISOP_CONFIG=../resources/config.cfg
			xterm -T $s -hold -geometry "120x25" -e "./Programa ../resources/$s.ansisop" &
		fi

		clear
		cd $ubicacion
		cd Programa/resources
		echo "Ingrese script a ejecutar (no hace falta .ansisop).  Ingrese '1' para agregar una CPU. Ingrese '0' para salir"
		echo " "
		ls
		read s

done

ps ax | grep xterm | grep resources/config.cfg | awk '{print $1}' | xargs kill
ps ax | grep xterm | grep ansisop | awk '{print $1}' | xargs kill
clear

exit 0